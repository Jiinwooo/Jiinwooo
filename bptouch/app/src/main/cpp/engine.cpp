// SPDX-License-Identifier: MIT OR Apache-2.0
#include "engine.h"
#include <algorithm>
#include <cmath>
#include <cstring>
#include <sstream>

#ifdef __ANDROID__
#include <android/log.h>
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, "BPTouch", __VA_ARGS__)
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, "BPTouch", __VA_ARGS__)
#else
#include <cstdio>
#define LOGE(...) (fprintf(stderr, __VA_ARGS__), fprintf(stderr, "\n"))
#define LOGI(...) (fprintf(stdout, __VA_ARGS__), fprintf(stdout, "\n"))
#endif

namespace bp {

// ======================================================================= histórico

struct HistEntry {
    enum Kind { Pixels, Selection, Structure, Doc, Compound, Marker } kind;
    std::string name;
    size_t bytes = 0;
    // Pixels
    int layerId = 0, x = 0, y = 0, w = 0, h = 0;
    std::vector<uint8_t> before, after;
    // Selection
    std::vector<uint8_t> selBefore, selAfter;
    bool hadSel = false, hasSel = false;
    // Structure
    std::vector<LayerMeta> metasBefore, metasAfter;
    int activeBefore = 0, activeAfter = 0;
    std::vector<std::pair<int, std::vector<uint8_t>>> store;
    // Doc (estado completo)
    int dw = 0, dh = 0;
    std::vector<LayerMeta> docMetas;
    int docActive = 0;
    std::vector<std::pair<int, std::vector<uint8_t>>> docPixels;
    std::vector<uint8_t> docSel;
    bool docHasSel = false;
    std::unique_ptr<HistEntry> docBefore, docAfter;
    // Compound
    std::vector<std::unique_ptr<HistEntry>> parts;

    HistEntry(Kind k, std::string n) : kind(k), name(std::move(n)) {}
    size_t size() const {
        size_t s = before.size() + after.size() + selBefore.size() + selAfter.size() + docSel.size();
        for (auto& p : store) s += p.second.size();
        for (auto& p : docPixels) s += p.second.size();
        if (docBefore) s += docBefore->size();
        if (docAfter) s += docAfter->size();
        for (auto& p : parts) s += p->size();
        return s;
    }
};

static constexpr size_t HIST_BUDGET = 700u * 1024u * 1024u;
static constexpr int HIST_MAX = 60;

// ======================================================================= básico GL

Engine::Engine(Loader loader) : loader_(std::move(loader)) {}

Engine::~Engine() {
    if (segThread_.joinable()) segThread_.join();
}

static GLuint compile(GLenum type, const std::string& src, const std::string& tag) {
    GLuint s = glCreateShader(type);
    const char* p = src.c_str();
    glShaderSource(s, 1, &p, nullptr);
    glCompileShader(s);
    GLint ok = 0;
    glGetShaderiv(s, GL_COMPILE_STATUS, &ok);
    if (!ok) {
        char log[2048]; glGetShaderInfoLog(s, sizeof log, nullptr, log);
        LOGE("shader %s: %s", tag.c_str(), log);
        glDeleteShader(s);
        return 0;
    }
    return s;
}

GLuint Engine::program(const std::string& vsName, const std::string& fsName) {
    std::string vs = loader_(vsName), fs = loader_(fsName);
    if (vs.empty() || fs.empty()) { LOGE("shader ausente: %s / %s", vsName.c_str(), fsName.c_str()); return 0; }
    GLuint v = compile(GL_VERTEX_SHADER, vs, vsName), f = compile(GL_FRAGMENT_SHADER, fs, fsName);
    if (!v || !f) return 0;
    GLuint p = glCreateProgram();
    glAttachShader(p, v); glAttachShader(p, f);
    glLinkProgram(p);
    glDeleteShader(v); glDeleteShader(f);
    GLint ok = 0;
    glGetProgramiv(p, GL_LINK_STATUS, &ok);
    if (!ok) {
        char log[2048]; glGetProgramInfoLog(p, sizeof log, nullptr, log);
        LOGE("link %s: %s", fsName.c_str(), log);
        return 0;
    }
    return p;
}

GLuint Engine::makeTex(int w, int h, GLenum ifmt, GLenum fmt, GLenum type, const void* data, bool linear) {
    GLuint t; glGenTextures(1, &t);
    glBindTexture(GL_TEXTURE_2D, t);
    glTexStorage2D(GL_TEXTURE_2D, 1, ifmt, w, h);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    if (data) glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, w, h, fmt, type, data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, linear ? GL_LINEAR : GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, linear ? GL_LINEAR : GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    return t;
}

GLuint Engine::makeLayerTex(const uint8_t* data) {
    GLuint t = makeTex(W_, H_, GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE, nullptr, true);
    if (data) writeTex(t, 0, 0, W_, H_, data);
    else { bindTarget(t, W_, H_); glClearColor(0, 0, 0, 0); glClear(GL_COLOR_BUFFER_BIT); }
    return t;
}

void Engine::bindTarget(GLuint tex, int w, int h) {
    glBindFramebuffer(GL_FRAMEBUFFER, fbo_);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex, 0);
    glViewport(0, 0, w, h);
}

void Engine::drawFS() {
    glBindVertexArray(vao_);
    glDrawArrays(GL_TRIANGLES, 0, 3);
}

void Engine::bindTex(int unit, GLuint tex, GLuint prog, const char* name) {
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, tex);
    glUniform1i(glGetUniformLocation(prog, name), unit);
}

void Engine::readTex(GLuint tex, int x, int y, int w, int h, std::vector<uint8_t>& out) {
    out.resize((size_t)w * h * 4);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo_);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex, 0);
    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glReadPixels(x, y, w, h, GL_RGBA, GL_UNSIGNED_BYTE, out.data());
}

void Engine::writeTex(GLuint tex, int x, int y, int w, int h, const uint8_t* data) {
    glBindTexture(GL_TEXTURE_2D, tex);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexSubImage2D(GL_TEXTURE_2D, 0, x, y, w, h, GL_RGBA, GL_UNSIGNED_BYTE, data);
}

static const float IDENT3[9] = {1, 0, 0, 0, 1, 0, 0, 0, 1};

void Engine::copyTex(GLuint src, GLuint dst) {
    bindTarget(dst, W_, H_);
    glDisable(GL_BLEND);
    glUseProgram(pCopy_);
    bindTex(0, src, pCopy_, "uSrc");
    bindTex(1, white1_, pCopy_, "uMask");
    glUniformMatrix3fv(glGetUniformLocation(pCopy_, "uXform"), 1, GL_FALSE, IDENT3);
    drawFS();
}

bool Engine::initGL() {
    const char* ver = (const char*)glGetString(GL_VERSION);
    const char* ren = (const char*)glGetString(GL_RENDERER);
    const char* ext = (const char*)glGetString(GL_EXTENSIONS);
    glInfo_ = std::string(ver ? ver : "?") + " · " + (ren ? ren : "?");
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTex_);
    std::string e = ext ? ext : "";
    GLint major = 3, minor = 0;
    glGetIntegerv(GL_MAJOR_VERSION, &major); glGetIntegerv(GL_MINOR_VERSION, &minor);
    halfFloatRT_ = e.find("GL_EXT_color_buffer_half_float") != std::string::npos ||
                   e.find("GL_EXT_color_buffer_float") != std::string::npos || (major == 3 && minor >= 2);

    const std::string fsv = "composite_blend.vert";
    pBlend_ = program(fsv, "composite_blend.frag");
    pCopy_ = program(fsv, "shaders/copy.frag");
    pThumb_ = program(fsv, "shaders/thumb.frag");
    pDab_ = program("shaders/dab.vert", "shaders/dab.frag");
    pStroke_ = program(fsv, "shaders/stroke.frag");
    pMove_ = program(fsv, "shaders/move.frag");
    pFill_ = program(fsv, "shaders/fill.frag");
    pBlur_ = program(fsv, "shaders/blur.frag");
    pAdjust_ = program(fsv, "shaders/adjust.frag");
    pDisplay_ = program("shaders/display.vert", "shaders/display.frag");
    if (!pBlend_ || !pCopy_ || !pThumb_ || !pDab_ || !pStroke_ || !pMove_ || !pFill_ || !pBlur_ || !pAdjust_ || !pDisplay_)
        return false;

    glGenVertexArrays(1, &vao_);
    glGenBuffers(1, &dabVbo_);
    glGenFramebuffers(1, &fbo_);
    uint8_t w[4] = {255, 255, 255, 255}, z[4] = {0, 0, 0, 0};
    white1_ = makeTex(1, 1, GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE, w, false);
    clear1_ = makeTex(1, 1, GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE, z, false);
    std::vector<uint8_t> lin(256);
    for (int i = 0; i < 256; ++i) lin[i] = (uint8_t)i;
    curveTex_ = makeTex(256, 1, GL_R8, GL_RED, GL_UNSIGNED_BYTE, lin.data(), true);
    LOGI("GL: %s · max tex %d · half-float RT %d", glInfo_.c_str(), maxTex_, (int)halfFloatRT_);
    return true;
}

void Engine::releaseGL() {
    // o contexto morreu: os nomes GL são inválidos, só esquecemos
    // nenhum nome antigo pode ser apagado depois: o novo contexto reutiliza os mesmos números
    if (segThread_.joinable()) segThread_.join();
    segBusy_ = false; segDone_ = false;
    layers_.clear(); W_ = H_ = 0; active_ = -1;
    compA_ = compB_ = compOut_ = preview_ = strokeTex_ = tmp_ = tmp2_ = region_ = selTex_ = 0;
    vao_ = dabVbo_ = fbo_ = curveTex_ = white1_ = clear1_ = 0;
    pBlend_ = pCopy_ = pThumb_ = pDab_ = pStroke_ = pMove_ = pFill_ = pBlur_ = pAdjust_ = pDisplay_ = 0;
    sel_ = Mask8(); hasSel_ = false; sampleValid_ = false; seg_.invalidate();
    hist_.clear(); histPos_ = 0; histBytes_ = 0;
    previewOn_ = false; adjType_ = -1; tool_ = Tool::Idle;
    bump();
}

void Engine::surfaceChanged(int w, int h) { screenW_ = std::max(1, w); screenH_ = std::max(1, h); }

void Engine::setView(const float m[6]) { std::memcpy(view_, m, sizeof view_); }

// ======================================================================= documento

void Engine::destroyDoc() {
    for (auto& l : layers_) glDeleteTextures(1, &l.tex);
    layers_.clear();
    GLuint t[] = {compA_, compB_, preview_, strokeTex_, tmp_, tmp2_, region_, selTex_};
    for (GLuint x : t) if (x) glDeleteTextures(1, &x);
    compA_ = compB_ = compOut_ = preview_ = strokeTex_ = tmp_ = tmp2_ = region_ = selTex_ = 0;
    active_ = -1;
}

void Engine::allocDocTextures() {
    bool hf = halfFloatRT_ && (int64_t)W_ * H_ <= 8'400'000;   // acima disso, RGBA8 para poupar memória de vídeo
    int levels = 1;
    for (int m = std::max(W_, H_); m > 1; m >>= 1) levels++;
    for (GLuint* t : {&compA_, &compB_}) {
        glGenTextures(1, t);
        glBindTexture(GL_TEXTURE_2D, *t);
        glTexStorage2D(GL_TEXTURE_2D, levels, hf ? GL_RGBA16F : GL_RGBA8, W_, H_);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }
    preview_ = makeTex(W_, H_, GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE, nullptr, true);
    tmp_ = makeTex(W_, H_, GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE, nullptr, true);
    tmp2_ = makeTex(W_, H_, GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE, nullptr, true);
    strokeTex_ = makeTex(W_, H_, GL_R8, GL_RED, GL_UNSIGNED_BYTE, nullptr, true);
    region_ = makeTex(W_, H_, GL_R8, GL_RED, GL_UNSIGNED_BYTE, nullptr, true);
    selTex_ = makeTex(W_, H_, GL_R8, GL_RED, GL_UNSIGNED_BYTE, nullptr, true);
    compOut_ = compA_;
}

static std::unique_ptr<HistEntry> marker(const char* n) { return std::make_unique<HistEntry>(HistEntry::Marker, n); }

bool Engine::newDocument(int w, int h, bool white) {
    if (segBusy_) return false;
    if (w <= 0 || h <= 0 || w > maxTex_ || h > maxTex_) return false;
    destroyDoc();
    W_ = w; H_ = h;
    allocDocTextures();
    std::vector<uint8_t> px;
    if (white) px.assign((size_t)w * h * 4, 255);
    Layer l; l.id = nextId_++; l.name = white ? "Fundo" : "Camada 1";
    l.tex = makeLayerTex(white ? px.data() : nullptr);
    layers_.push_back(l); active_ = 0;
    sel_ = Mask8(); hasSel_ = false; sampleValid_ = false; seg_.invalidate();
    hist_.clear(); histPos_ = 0; histBytes_ = 0;
    push(marker("Novo"));
    docDirty_ = true; previewOn_ = false; adjType_ = -1;
    bump();
    return true;
}

bool Engine::openImage(const uint8_t* rgba, int w, int h, const std::string& name) {
    if (segBusy_) return false;
    if (w <= 0 || h <= 0 || w > maxTex_ || h > maxTex_) return false;
    destroyDoc();
    W_ = w; H_ = h;
    allocDocTextures();
    Layer l; l.id = nextId_++; l.name = "Fundo";
    l.tex = makeLayerTex(rgba);
    layers_.push_back(l); active_ = 0;
    sel_ = Mask8(); hasSel_ = false; sampleValid_ = false; seg_.invalidate();
    hist_.clear(); histPos_ = 0; histBytes_ = 0;
    push(marker("Abrir"));
    docDirty_ = true; previewOn_ = false; adjType_ = -1;
    (void)name;
    bump();
    return true;
}

bool Engine::addImageLayer(const uint8_t* rgba, int w, int h, const std::string& name) {
    if (!W_ || segBusy_) return false;
    auto before = metas(); int ab = activeLayerId();
    // centraliza a imagem no documento (corta o que sobrar)
    std::vector<uint8_t> px((size_t)W_ * H_ * 4, 0);
    int ox = (W_ - w) / 2, oy = (H_ - h) / 2;
    for (int y = 0; y < h; ++y) {
        int dy = y + oy; if (dy < 0 || dy >= H_) continue;
        for (int x = 0; x < w; ++x) {
            int dx = x + ox; if (dx < 0 || dx >= W_) continue;
            std::memcpy(&px[((size_t)dy * W_ + dx) * 4], &rgba[((size_t)y * w + x) * 4], 4);
        }
    }
    Layer l; l.id = nextId_++; l.name = name.empty() ? "Imagem" : name;
    l.tex = makeLayerTex(px.data());
    layers_.insert(layers_.begin() + active_ + 1, l);
    active_ = active_ + 1;
    pushStructure("Inserir imagem", before, ab, {}, {});
    layerChanged();
    return true;
}

bool Engine::exportComposite(std::vector<uint8_t>& out) {
    if (!W_) return false;
    if (docDirty_) composite();
    // composto (talvez 16F) → RGBA8 → CPU
    bindTarget(tmp_, W_, H_);
    glDisable(GL_BLEND);
    glUseProgram(pCopy_);
    bindTex(0, compOut_, pCopy_, "uSrc");
    bindTex(1, white1_, pCopy_, "uMask");
    glUniformMatrix3fv(glGetUniformLocation(pCopy_, "uXform"), 1, GL_FALSE, IDENT3);
    glActiveTexture(GL_TEXTURE0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    drawFS();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    readTex(tmp_, 0, 0, W_, H_, out);
    return true;
}

// ======================================================================= composição e tela

void Engine::composite() {
    if (!W_) return;
    glDisable(GL_BLEND);
    glDisable(GL_SCISSOR_TEST);
    GLuint cur = compA_, other = compB_;
    bindTarget(cur, W_, H_);
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT);
    glUseProgram(pBlend_);
    GLint uMode = glGetUniformLocation(pBlend_, "uMode"), uOp = glGetUniformLocation(pBlend_, "uOpacity"),
          uFill = glGetUniformLocation(pBlend_, "uFill"), uSeed = glGetUniformLocation(pBlend_, "uDissolveSeed");
    for (int i = 0; i < (int)layers_.size(); ++i) {
        const Layer& l = layers_[i];
        if (!l.visible) continue;
        GLuint src = (i == active_ && previewOn_) ? preview_ : l.tex;
        bindTarget(other, W_, H_);
        bindTex(0, cur, pBlend_, "uBackdrop");
        bindTex(1, src, pBlend_, "uLayer");
        bindTex(2, white1_, pBlend_, "uMask");
        glUniform1i(uMode, l.blend);
        glUniform1f(uOp, l.opacity);
        glUniform1f(uFill, 1.f);
        glUniform1ui(uSeed, (GLuint)l.id * 2654435761u);
        // o fundo é lido com mip 0 mesmo com filtro de mipmap
        glActiveTexture(GL_TEXTURE0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        drawFS();
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        std::swap(cur, other);
    }
    compOut_ = cur;
    glBindTexture(GL_TEXTURE_2D, compOut_);
    glGenerateMipmap(GL_TEXTURE_2D);
    docDirty_ = false;
}

bool Engine::render(float t) {
    finishAsyncSelection();
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, screenW_, screenH_);
    glClearColor(0.118f, 0.118f, 0.118f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT);
    if (!W_ || !pDisplay_) return false;
    if (docDirty_) composite();
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, screenW_, screenH_);
    glDisable(GL_BLEND);
    glUseProgram(pDisplay_);
    float m[9] = {view_[0], view_[1], 0, view_[2], view_[3], 0, view_[4], view_[5], 1};
    glUniformMatrix3fv(glGetUniformLocation(pDisplay_, "uDocToScreen"), 1, GL_FALSE, m);
    glUniform2f(glGetUniformLocation(pDisplay_, "uDocSize"), (float)W_, (float)H_);
    glUniform2f(glGetUniformLocation(pDisplay_, "uScreen"), (float)screenW_, (float)screenH_);
    glUniform1i(glGetUniformLocation(pDisplay_, "uHasSel"), hasSel_ ? 1 : 0);
    glUniform1f(glGetUniformLocation(pDisplay_, "uTime"), std::fmod(t, 1000.f));
    float zoom = std::sqrt(std::fabs(view_[0] * view_[3] - view_[1] * view_[2]));
    glUniform1f(glGetUniformLocation(pDisplay_, "uPixelGrid"), zoom >= 12.f ? 1.f : 0.f);
    bindTex(0, compOut_, pDisplay_, "uComp");
    bindTex(1, selOrWhite(), pDisplay_, "uSel");
    glBindVertexArray(vao_);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    return hasSel_ || segBusy_;
}

// ======================================================================= camadas

int Engine::layerIndex(int id) const {
    for (int i = 0; i < (int)layers_.size(); ++i) if (layers_[i].id == id) return i;
    return -1;
}

int Engine::activeLayerId() const { return active_ >= 0 && active_ < (int)layers_.size() ? layers_[active_].id : 0; }

static std::string jsonEscape(const std::string& s) {
    std::string o;
    for (char c : s) { if (c == '"' || c == '\\') o += '\\'; if ((unsigned char)c >= 0x20) o += c; }
    return o;
}

std::string Engine::layersJson() const {
    std::ostringstream o;
    o << "{\"w\":" << W_ << ",\"h\":" << H_ << ",\"active\":" << activeLayerId() << ",\"sel\":" << (hasSel_ ? 1 : 0)
      << ",\"busy\":" << (segBusy_ ? 1 : 0) << ",\"canUndo\":" << (histPos_ > 1 ? 1 : 0)
      << ",\"canRedo\":" << (histPos_ < (int)hist_.size() ? 1 : 0) << ",\"layers\":[";
    for (int i = (int)layers_.size() - 1; i >= 0; --i) {   // de cima para baixo, como o painel
        const Layer& l = layers_[i];
        o << "{\"id\":" << l.id << ",\"name\":\"" << jsonEscape(l.name) << "\",\"visible\":" << (l.visible ? 1 : 0)
          << ",\"opacity\":" << l.opacity << ",\"blend\":" << l.blend << ",\"lockAlpha\":" << (l.lockAlpha ? 1 : 0) << "}";
        if (i) o << ",";
    }
    o << "]}";
    return o.str();
}

std::string Engine::historyJson() const {
    std::ostringstream o;
    o << "{\"pos\":" << histPos_ << ",\"items\":[";
    for (size_t i = 0; i < hist_.size(); ++i) { o << "\"" << jsonEscape(hist_[i]->name) << "\""; if (i + 1 < hist_.size()) o << ","; }
    o << "]}";
    return o.str();
}

std::vector<LayerMeta> Engine::metas() const {
    std::vector<LayerMeta> m;
    for (auto& l : layers_) m.push_back({l.id, l.name, l.visible, l.opacity, l.blend, l.lockAlpha});
    return m;
}

void Engine::layerChanged() {
    sampleValid_ = false;
    if (!segBusy_) seg_.invalidate();
    docDirty_ = true;
    bump();
}

void Engine::setActiveLayer(int id) {
    int i = layerIndex(id);
    if (i < 0 || i == active_ || previewOn_) return;
    active_ = i;
    sampleValid_ = false;
    if (!segBusy_) seg_.invalidate();
    bump();
}

void Engine::addLayer() {
    if (!W_ || previewOn_) return;
    auto before = metas(); int ab = activeLayerId();
    Layer l; l.id = nextId_++;
    int n = 1; for (auto& x : layers_) if (x.name.rfind("Camada ", 0) == 0) n = std::max(n, atoi(x.name.c_str() + 7) + 1);
    l.name = "Camada " + std::to_string(n);
    l.tex = makeLayerTex(nullptr);
    layers_.insert(layers_.begin() + active_ + 1, l);
    active_++;
    pushStructure("Nova camada", before, ab, {}, {});
    layerChanged();
}

void Engine::duplicateLayer() {
    if (active_ < 0 || previewOn_) return;
    auto before = metas(); int ab = activeLayerId();
    Layer l = layers_[active_];
    l.id = nextId_++;
    l.name = layers_[active_].name + " cópia";
    l.tex = makeLayerTex(nullptr);
    copyTex(layers_[active_].tex, l.tex);
    layers_.insert(layers_.begin() + active_ + 1, l);
    active_++;
    pushStructure("Duplicar camada", before, ab, {}, {});
    layerChanged();
}

void Engine::deleteLayer() {
    if (layers_.size() <= 1 || previewOn_) return;
    auto before = metas(); int ab = activeLayerId();
    std::vector<uint8_t> px;
    readTex(layers_[active_].tex, 0, 0, W_, H_, px);
    int id = layers_[active_].id;
    glDeleteTextures(1, &layers_[active_].tex);
    layers_.erase(layers_.begin() + active_);
    active_ = std::max(0, active_ - 1);
    pushStructure("Excluir camada", before, ab, {id}, {std::move(px)});
    layerChanged();
}

void Engine::moveLayer(int delta) {
    int j = active_ + delta;
    if (active_ < 0 || j < 0 || j >= (int)layers_.size() || previewOn_) return;
    auto before = metas(); int ab = activeLayerId();
    std::swap(layers_[active_], layers_[j]);
    active_ = j;
    pushStructure(delta > 0 ? "Trazer para frente" : "Enviar para trás", before, ab, {}, {});
    layerChanged();
}

void Engine::setLayerVisible(int id, bool v) {
    int i = layerIndex(id);
    if (i < 0 || layers_[i].visible == v) return;
    auto before = metas(); int ab = activeLayerId();
    layers_[i].visible = v;
    pushStructure(v ? "Mostrar camada" : "Ocultar camada", before, ab, {}, {});
    docDirty_ = true; bump();
}

void Engine::setLayerOpacity(float o, bool final) {
    if (active_ < 0) return;
    static std::vector<LayerMeta> before;
    static int ab = 0;
    static bool pending = false;
    if (!pending) { before = metas(); ab = activeLayerId(); pending = true; }
    layers_[active_].opacity = std::clamp(o, 0.f, 1.f);
    docDirty_ = true;
    if (final) {
        pending = false;
        pushStructure("Opacidade da camada", before, ab, {}, {});
    }
    bump();
}

void Engine::setLayerBlend(int mode) {
    if (active_ < 0 || layers_[active_].blend == mode) return;
    auto before = metas(); int ab = activeLayerId();
    layers_[active_].blend = std::clamp(mode, 0, 26);
    pushStructure("Modo de mesclagem", before, ab, {}, {});
    docDirty_ = true; bump();
}

void Engine::setLayerLockAlpha(bool v) {
    if (active_ < 0) return;
    auto before = metas(); int ab = activeLayerId();
    layers_[active_].lockAlpha = v;
    pushStructure(v ? "Bloquear transparência" : "Desbloquear transparência", before, ab, {}, {});
    bump();
}

void Engine::renameLayer(int id, const std::string& name) {
    int i = layerIndex(id);
    if (i < 0 || name.empty()) return;
    auto before = metas(); int ab = activeLayerId();
    layers_[i].name = name;
    pushStructure("Renomear camada", before, ab, {}, {});
    bump();
}

void Engine::mergeDown() {
    if (active_ <= 0 || previewOn_) return;
    auto e = std::make_unique<HistEntry>(HistEntry::Compound, "Mesclar para baixo");
    Layer& top = layers_[active_];
    Layer& bot = layers_[active_ - 1];
    // pixels da camada de baixo antes
    auto pe = std::make_unique<HistEntry>(HistEntry::Pixels, "");
    pe->layerId = bot.id; pe->x = 0; pe->y = 0; pe->w = W_; pe->h = H_;
    readTex(bot.tex, 0, 0, W_, H_, pe->before);
    if (top.visible) {
        bindTarget(tmp_, W_, H_);
        glDisable(GL_BLEND);
        glUseProgram(pBlend_);
        bindTex(0, bot.tex, pBlend_, "uBackdrop");
        bindTex(1, top.tex, pBlend_, "uLayer");
        bindTex(2, white1_, pBlend_, "uMask");
        glUniform1i(glGetUniformLocation(pBlend_, "uMode"), top.blend);
        glUniform1f(glGetUniformLocation(pBlend_, "uOpacity"), top.opacity);
        glUniform1f(glGetUniformLocation(pBlend_, "uFill"), 1.f);
        glUniform1ui(glGetUniformLocation(pBlend_, "uDissolveSeed"), (GLuint)top.id * 2654435761u);
        drawFS();
        std::swap(tmp_, bot.tex);
    }
    readTex(bot.tex, 0, 0, W_, H_, pe->after);
    // estrutura: remove a de cima
    auto before = metas(); int ab = activeLayerId();
    std::vector<uint8_t> px; readTex(top.tex, 0, 0, W_, H_, px);
    int id = top.id;
    glDeleteTextures(1, &top.tex);
    layers_.erase(layers_.begin() + active_);
    active_--;
    auto se = std::make_unique<HistEntry>(HistEntry::Structure, "");
    se->metasBefore = before; se->activeBefore = ab;
    se->metasAfter = metas(); se->activeAfter = activeLayerId();
    se->store.push_back({id, std::move(px)});
    e->parts.push_back(std::move(pe));
    e->parts.push_back(std::move(se));
    push(std::move(e));
    layerChanged();
}

void Engine::flatten() {
    if (layers_.size() <= 1 || previewOn_) return;
    auto before = snapshotDoc();
    if (docDirty_) composite();
    std::vector<uint8_t> px;
    exportComposite(px);
    // fundo branco por baixo, como "Achatar imagem"
    for (size_t i = 0; i < px.size(); i += 4) {
        int a = px[i + 3];
        for (int c = 0; c < 3; ++c) px[i + c] = (uint8_t)std::min(255, px[i + c] + (255 - a));
        px[i + 3] = 255;
    }
    for (auto& l : layers_) glDeleteTextures(1, &l.tex);
    layers_.clear();
    Layer l; l.id = nextId_++; l.name = "Fundo"; l.tex = makeLayerTex(px.data());
    layers_.push_back(l); active_ = 0;
    pushDocSnapshot("Achatar imagem", std::move(before));
    layerChanged();
}

bool Engine::layerThumbnail(int id, int tw, int th, std::vector<uint8_t>& out) {
    int i = layerIndex(id);
    if (i < 0 || tw <= 0 || th <= 0) return false;
    GLuint t = makeTex(tw, th, GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE, nullptr, false);
    bindTarget(t, tw, th);
    glDisable(GL_BLEND);
    glUseProgram(pThumb_);
    GLuint src = (i == active_ && previewOn_) ? preview_ : layers_[i].tex;
    bindTex(0, src, pThumb_, "uSrc");
    glUniform2f(glGetUniformLocation(pThumb_, "uOutSize"), (float)tw, (float)th);
    drawFS();
    readTex(t, 0, 0, tw, th, out);
    glDeleteTextures(1, &t);
    return true;
}

// ======================================================================= pincel

void Engine::strokeBegin(Tool t, float x, float y, float p, float size, float hard, float opacity,
                         const float rgba[4], bool prSize, bool prOp) {
    if (active_ < 0 || previewOn_ || segBusy_) return;
    if (t == Tool::Clone) {
        if (!cloneSrcSet_) return;
        if (!cloneAligned_) { cloneOffX_ = x - cloneSrcX_; cloneOffY_ = y - cloneSrcY_; cloneAligned_ = true; }
    }
    tool_ = t;
    bSize_ = std::max(1.f, size); bHard_ = std::clamp(hard, 0.f, 1.f); bOpacity_ = std::clamp(opacity, 0.f, 1.f);
    std::memcpy(bColor_, rgba, sizeof bColor_);
    bPrSize_ = prSize; bPrOpacity_ = prOp;
    // traço vazio + prévia = camada
    bindTarget(strokeTex_, W_, H_);
    glDisable(GL_SCISSOR_TEST);
    glClearColor(0, 0, 0, 0); glClear(GL_COLOR_BUFFER_BIT);
    copyTex(layers_[active_].tex, preview_);
    sx0_ = W_; sy0_ = H_; sx1_ = 0; sy1_ = 0;
    dabs_.clear(); carry_ = 0;
    lastX_ = x; lastY_ = y; lastP_ = p;
    previewOn_ = true;
    stampTo(x, y, p);
    flushDabs();
}

void Engine::stampTo(float x, float y, float p) {
    float r = bSize_ * 0.5f * (bPrSize_ ? std::max(0.05f, p) : 1.f);
    float a = bOpacity_ * (bPrOpacity_ ? p : 1.f);
    dabs_.insert(dabs_.end(), {x, y, std::max(0.5f, r), a});
    int pad = (int)std::ceil(r) + 2;
    sx0_ = std::max(0, std::min(sx0_, (int)x - pad)); sy0_ = std::max(0, std::min(sy0_, (int)y - pad));
    sx1_ = std::min(W_, std::max(sx1_, (int)x + pad + 1)); sy1_ = std::min(H_, std::max(sy1_, (int)y + pad + 1));
}

void Engine::strokeMove(float x, float y, float p) {
    if (tool_ == Tool::Idle || !previewOn_) return;
    float dx = x - lastX_, dy = y - lastY_;
    float dist = std::hypot(dx, dy);
    float spacing = std::max(0.75f, bSize_ * 0.08f);
    float d = carry_;
    if (dist > 0) {
        while (d + spacing <= dist) {
            d += spacing;
            float t = d / dist;
            stampTo(lastX_ + dx * t, lastY_ + dy * t, lastP_ + (p - lastP_) * t);
        }
        carry_ = d - dist;
        if (carry_ < -spacing) carry_ = 0;
    }
    lastX_ = x; lastY_ = y; lastP_ = p;
    flushDabs();
}

void Engine::flushDabs() {
    if (dabs_.empty()) return;
    // 1) carimbos → máscara do traço (máximo: a opacidade limita o traço inteiro)
    bindTarget(strokeTex_, W_, H_);
    glEnable(GL_BLEND);
    glBlendEquation(GL_MAX);
    glBlendFunc(GL_ONE, GL_ONE);
    glUseProgram(pDab_);
    glUniform2f(glGetUniformLocation(pDab_, "uDocSize"), (float)W_, (float)H_);
    glUniform1f(glGetUniformLocation(pDab_, "uHardness"), bHard_);
    glBindVertexArray(vao_);
    glBindBuffer(GL_ARRAY_BUFFER, dabVbo_);
    glBufferData(GL_ARRAY_BUFFER, dabs_.size() * sizeof(float), dabs_.data(), GL_STREAM_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, nullptr);
    glVertexAttribDivisor(0, 1);
    glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, (GLsizei)(dabs_.size() / 4));
    glVertexAttribDivisor(0, 0);
    glDisableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBlendEquation(GL_FUNC_ADD);
    glDisable(GL_BLEND);
    dabs_.clear();
    renderStrokePreview();
}

void Engine::renderStrokePreview() {
    if (sx1_ <= sx0_ || sy1_ <= sy0_) return;
    bindTarget(preview_, W_, H_);
    glEnable(GL_SCISSOR_TEST);
    glScissor(sx0_, sy0_, sx1_ - sx0_, sy1_ - sy0_);
    glUseProgram(pStroke_);
    bindTex(0, layers_[active_].tex, pStroke_, "uLayer");
    bindTex(1, strokeTex_, pStroke_, "uStroke");
    bindTex(2, selOrWhite(), pStroke_, "uSel");
    int mode = tool_ == Tool::Eraser ? 1 : tool_ == Tool::Clone ? 2 : 0;
    glUniform1i(glGetUniformLocation(pStroke_, "uMode"), mode);
    glUniform4fv(glGetUniformLocation(pStroke_, "uColor"), 1, bColor_);
    glUniform2f(glGetUniformLocation(pStroke_, "uCloneOffset"), cloneOffX_ / W_, cloneOffY_ / H_);
    glUniform1i(glGetUniformLocation(pStroke_, "uLockAlpha"), layers_[active_].lockAlpha ? 1 : 0);
    drawFS();
    glDisable(GL_SCISSOR_TEST);
    docDirty_ = true;
}

void Engine::commitPreviewToLayer(const char* name, int x0, int y0, int x1, int y1) {
    x0 = std::clamp(x0, 0, W_); x1 = std::clamp(x1, 0, W_); y0 = std::clamp(y0, 0, H_); y1 = std::clamp(y1, 0, H_);
    previewOn_ = false;
    if (x1 <= x0 || y1 <= y0) { docDirty_ = true; bump(); return; }
    auto e = std::make_unique<HistEntry>(HistEntry::Pixels, name);
    e->layerId = layers_[active_].id; e->x = x0; e->y = y0; e->w = x1 - x0; e->h = y1 - y0;
    readTex(layers_[active_].tex, x0, y0, e->w, e->h, e->before);
    readTex(preview_, x0, y0, e->w, e->h, e->after);
    // a prévia já é a camada nova: troca os nomes das texturas, e a prévia é refeita na próxima vez
    std::swap(layers_[active_].tex, preview_);
    push(std::move(e));
    layerChanged();
}

void Engine::strokeEnd() {
    if (tool_ == Tool::Idle || !previewOn_) { tool_ = Tool::Idle; return; }
    flushDabs();
    const char* n = tool_ == Tool::Eraser ? "Borracha" : tool_ == Tool::Clone ? "Carimbo" : "Pincel";
    tool_ = Tool::Idle;
    commitPreviewToLayer(n, sx0_, sy0_, sx1_, sy1_);
}

void Engine::strokeCancel() {
    if (tool_ != Tool::Brush && tool_ != Tool::Eraser && tool_ != Tool::Clone) return;
    tool_ = Tool::Idle; previewOn_ = false; dabs_.clear(); docDirty_ = true;
}

void Engine::moveCancel() {
    if (tool_ != Tool::Move) return;
    tool_ = Tool::Idle; previewOn_ = false; docDirty_ = true;
}

// ======================================================================= mover

void Engine::moveBegin() {
    if (active_ < 0 || previewOn_ || segBusy_) return;
    tool_ = Tool::Move;
    moveDx_ = moveDy_ = 0;
    previewOn_ = true;
    moveUpdate(0, 0);
}

void Engine::moveUpdate(float dx, float dy) {
    if (tool_ != Tool::Move) return;
    moveDx_ = std::round(dx); moveDy_ = std::round(dy);
    bindTarget(preview_, W_, H_);
    glDisable(GL_BLEND);
    glUseProgram(pMove_);
    bindTex(0, layers_[active_].tex, pMove_, "uLayer");
    bindTex(1, selOrWhite(), pMove_, "uSel");
    glUniform2f(glGetUniformLocation(pMove_, "uOffset"), moveDx_ / W_, moveDy_ / H_);
    drawFS();
    docDirty_ = true;
}

void Engine::moveEnd() {
    if (tool_ != Tool::Move) return;
    tool_ = Tool::Idle;
    int dx = (int)moveDx_, dy = (int)moveDy_;
    if (!dx && !dy) { previewOn_ = false; docDirty_ = true; return; }
    int x0 = 0, y0 = 0, x1 = W_, y1 = H_;
    if (hasSel_ && selectionBounds(x0, y0, x1, y1)) {
        x0 = std::min(x0, x0 + dx); y0 = std::min(y0, y0 + dy);
        x1 = std::max(x1, x1 + dx); y1 = std::max(y1, y1 + dy);
    }
    auto comp = std::make_unique<HistEntry>(HistEntry::Compound, "Mover");
    // pixels
    x0 = std::clamp(x0, 0, W_); x1 = std::clamp(x1, 0, W_); y0 = std::clamp(y0, 0, H_); y1 = std::clamp(y1, 0, H_);
    auto e = std::make_unique<HistEntry>(HistEntry::Pixels, "");
    e->layerId = layers_[active_].id; e->x = x0; e->y = y0; e->w = x1 - x0; e->h = y1 - y0;
    if (e->w > 0 && e->h > 0) {
        readTex(layers_[active_].tex, x0, y0, e->w, e->h, e->before);
        readTex(preview_, x0, y0, e->w, e->h, e->after);
    }
    std::swap(layers_[active_].tex, preview_);
    previewOn_ = false;
    comp->parts.push_back(std::move(e));
    // a seleção acompanha o conteúdo
    if (hasSel_) {
        auto s = std::make_unique<HistEntry>(HistEntry::Selection, "");
        s->hadSel = true; s->selBefore = rleEncode(sel_);
        Mask8 m; m.reset(W_, H_, 0);
        for (int y = 0; y < H_; ++y) {
            int sy = y - dy; if (sy < 0 || sy >= H_) continue;
            for (int x = 0; x < W_; ++x) { int sx = x - dx; if (sx >= 0 && sx < W_) m.v[(size_t)y * W_ + x] = sel_.v[(size_t)sy * W_ + sx]; }
        }
        sel_ = std::move(m);
        hasSel_ = !maskIsEmpty(sel_);
        s->hasSel = hasSel_; s->selAfter = rleEncode(sel_);
        uploadSel();
        comp->parts.push_back(std::move(s));
    }
    push(std::move(comp));
    layerChanged();
}

// ======================================================================= preencher / degradê / conta-gotas

static void setFillCommon(GLuint p, int W, int H) {
    glUniform2f(glGetUniformLocation(p, "uDocSize"), (float)W, (float)H);
}

void Engine::gradientPreview(float x0, float y0, float x1, float y1, const float a[4], const float b[4], bool radial) {
    if (active_ < 0 || segBusy_) return;
    if (!previewOn_) { previewOn_ = true; tool_ = Tool::Gradient; }
    if (tool_ != Tool::Gradient) return;
    gradP_[0] = x0; gradP_[1] = y0; gradP_[2] = x1; gradP_[3] = y1;
    std::memcpy(gradA_, a, sizeof gradA_); std::memcpy(gradB_, b, sizeof gradB_); gradRadial_ = radial;
    bindTarget(preview_, W_, H_);
    glDisable(GL_BLEND);
    glUseProgram(pFill_);
    bindTex(0, layers_[active_].tex, pFill_, "uLayer");
    bindTex(1, selOrWhite(), pFill_, "uSel");
    bindTex(2, white1_, pFill_, "uRegion");
    glUniform1i(glGetUniformLocation(pFill_, "uMode"), radial ? 3 : 2);
    glUniform4fv(glGetUniformLocation(pFill_, "uColorA"), 1, a);
    glUniform4fv(glGetUniformLocation(pFill_, "uColorB"), 1, b);
    glUniform2f(glGetUniformLocation(pFill_, "uP0"), x0, y0);
    glUniform2f(glGetUniformLocation(pFill_, "uP1"), x1, y1);
    setFillCommon(pFill_, W_, H_);
    drawFS();
    docDirty_ = true;
}

void Engine::gradientCommit() {
    if (tool_ != Tool::Gradient) return;
    tool_ = Tool::Idle;
    int x0 = 0, y0 = 0, x1 = W_, y1 = H_;
    if (hasSel_) selectionBounds(x0, y0, x1, y1);
    commitPreviewToLayer("Degradê", x0, y0, x1, y1);
}

void Engine::gradientCancel() {
    if (tool_ != Tool::Gradient) return;
    tool_ = Tool::Idle; previewOn_ = false; docDirty_ = true;
}

void Engine::fillSelection(const float rgba[4]) {
    if (active_ < 0 || previewOn_ || segBusy_) return;
    bindTarget(preview_, W_, H_);
    glDisable(GL_BLEND);
    glUseProgram(pFill_);
    bindTex(0, layers_[active_].tex, pFill_, "uLayer");
    bindTex(1, selOrWhite(), pFill_, "uSel");
    bindTex(2, white1_, pFill_, "uRegion");
    glUniform1i(glGetUniformLocation(pFill_, "uMode"), 0);
    glUniform4fv(glGetUniformLocation(pFill_, "uColorA"), 1, rgba);
    setFillCommon(pFill_, W_, H_);
    drawFS();
    previewOn_ = true;
    int x0 = 0, y0 = 0, x1 = W_, y1 = H_;
    if (hasSel_) selectionBounds(x0, y0, x1, y1);
    commitPreviewToLayer("Preencher", x0, y0, x1, y1);
}

void Engine::clearSelectionPixels() {
    if (active_ < 0 || previewOn_ || segBusy_) return;
    bindTarget(preview_, W_, H_);
    glDisable(GL_BLEND);
    glUseProgram(pFill_);
    bindTex(0, layers_[active_].tex, pFill_, "uLayer");
    bindTex(1, selOrWhite(), pFill_, "uSel");
    bindTex(2, white1_, pFill_, "uRegion");
    glUniform1i(glGetUniformLocation(pFill_, "uMode"), 1);
    setFillCommon(pFill_, W_, H_);
    drawFS();
    previewOn_ = true;
    int x0 = 0, y0 = 0, x1 = W_, y1 = H_;
    if (hasSel_) selectionBounds(x0, y0, x1, y1);
    commitPreviewToLayer("Limpar", x0, y0, x1, y1);
}

void Engine::bucketFill(float x, float y, int tol, bool contiguous, const float rgba[4]) {
    if (active_ < 0 || previewOn_ || segBusy_) return;
    ensureSample(false);
    Mask8 reg;
    bp::magicWand(reg, sample_, (int)x, (int)y, tol, contiguous, true);
    int x0, y0, x1, y1;
    if (!maskBounds(reg, x0, y0, x1, y1)) return;
    glBindTexture(GL_TEXTURE_2D, region_);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, W_, H_, GL_RED, GL_UNSIGNED_BYTE, reg.v.data());
    bindTarget(preview_, W_, H_);
    glDisable(GL_BLEND);
    glUseProgram(pFill_);
    bindTex(0, layers_[active_].tex, pFill_, "uLayer");
    bindTex(1, selOrWhite(), pFill_, "uSel");
    bindTex(2, region_, pFill_, "uRegion");
    glUniform1i(glGetUniformLocation(pFill_, "uMode"), 0);
    glUniform4fv(glGetUniformLocation(pFill_, "uColorA"), 1, rgba);
    setFillCommon(pFill_, W_, H_);
    drawFS();
    previewOn_ = true;
    commitPreviewToLayer("Lata de tinta", x0, y0, x1, y1);
}

uint32_t Engine::sampleColor(float x, float y) {
    if (!W_) return 0xFF000000u;
    int ix = std::clamp((int)x, 0, W_ - 1), iy = std::clamp((int)y, 0, H_ - 1);
    if (docDirty_) composite();
    bindTarget(tmp_, W_, H_);
    glEnable(GL_SCISSOR_TEST);
    glScissor(ix, iy, 1, 1);
    glUseProgram(pCopy_);
    bindTex(0, compOut_, pCopy_, "uSrc");
    bindTex(1, white1_, pCopy_, "uMask");
    glUniformMatrix3fv(glGetUniformLocation(pCopy_, "uXform"), 1, GL_FALSE, IDENT3);
    glActiveTexture(GL_TEXTURE0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    drawFS();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glDisable(GL_SCISSOR_TEST);
    std::vector<uint8_t> px;
    readTex(tmp_, ix, iy, 1, 1, px);
    int a = px[3];
    if (a == 0) return 0x00000000u;
    int r = std::min(255, px[0] * 255 / a), g = std::min(255, px[1] * 255 / a), b = std::min(255, px[2] * 255 / a);
    return ((uint32_t)a << 24) | (r << 16) | (g << 8) | b;
}

// ======================================================================= seleção

void Engine::uploadSel() {
    if (!selTex_) return;
    glBindTexture(GL_TEXTURE_2D, selTex_);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    if (hasSel_ && sel_.w == W_ && sel_.h == H_)
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, W_, H_, GL_RED, GL_UNSIGNED_BYTE, sel_.v.data());
}

bool Engine::selectionBounds(int& x0, int& y0, int& x1, int& y1) const {
    if (!hasSel_) return false;
    return maskBounds(sel_, x0, y0, x1, y1);
}

void Engine::applySelection(const Mask8& m, const char* name) {
    auto e = std::make_unique<HistEntry>(HistEntry::Selection, name);
    e->hadSel = hasSel_;
    if (hasSel_) e->selBefore = rleEncode(sel_);
    sel_ = m;
    hasSel_ = !sel_.empty() && !maskIsEmpty(sel_);
    e->hasSel = hasSel_;
    if (hasSel_) e->selAfter = rleEncode(sel_);
    uploadSel();
    push(std::move(e));
    bump();
}

static SelOp toOp(int op) { return op == 1 ? SelOp::Add : op == 2 ? SelOp::Subtract : op == 3 ? SelOp::Intersect : SelOp::Replace; }

void Engine::selectPolygon(const std::vector<Pt>& pts, int op, float feather, bool aa, const char* name) {
    if (!W_ || segBusy_) return;
    Mask8 add;
    rasterPolygon(add, W_, H_, pts, aa);
    featherMask(add, feather);
    Mask8 cur = hasSel_ ? sel_ : Mask8();
    combineMask(cur, add, toOp(op));
    applySelection(cur, name);
}

void Engine::selectEllipse(float x0, float y0, float x1, float y1, int op, float feather, bool aa) {
    std::vector<Pt> pts;
    ellipsePolygon(pts, x0, y0, x1, y1);
    selectPolygon(pts, op, feather, aa, "Letreiro elíptico");
}

void Engine::ensureSample(bool all) {
    if (sampleValid_ && sampleAll_ == all && sample_.w == W_ && sample_.h == H_) return;
    sample_.w = W_; sample_.h = H_;
    if (all) exportComposite(sample_.px);
    else readTex(layers_[active_].tex, 0, 0, W_, H_, sample_.px);
    sampleValid_ = true; sampleAll_ = all;
    seg_.invalidate();
}

void Engine::magicWand(float x, float y, int tol, bool contiguous, bool aa, bool all, int op) {
    if (!W_ || segBusy_) return;
    ensureSample(all);
    Mask8 add;
    bp::magicWand(add, sample_, (int)x, (int)y, tol, contiguous, aa);
    Mask8 cur = hasSel_ ? sel_ : Mask8();
    combineMask(cur, add, toOp(op));
    applySelection(cur, "Varinha mágica");
}

void Engine::quickSelect(const std::vector<Pt>& stroke, float radius, bool subtract, bool all) {
    if (!W_ || segBusy_ || stroke.empty()) return;
    ensureSample(all);
    segBefore_ = hasSel_ ? sel_ : Mask8();
    segResult_ = segBefore_;
    segName_ = "Seleção rápida";
    segBusy_ = true; segDone_ = false;
    if (segThread_.joinable()) segThread_.join();
    QuickSelectParams p; p.stroke = stroke; p.brushRadius = radius; p.subtract = subtract;
    segThread_ = std::thread([this, p]() {
        if (!seg_.ready()) seg_.prepare(sample_);
        if (segResult_.empty()) segResult_.reset(W_, H_, 0);
        seg_.quickSelect(segResult_, sample_, p);
        segDone_ = true;
    });
    bump();
}

void Engine::objectSelect(const std::vector<Pt>& region, bool all, int op) {
    if (!W_ || segBusy_ || region.size() < 3) return;
    ensureSample(all);
    segBefore_ = hasSel_ ? sel_ : Mask8();
    segName_ = "Seleção de objeto";
    segBusy_ = true; segDone_ = false;
    if (segThread_.joinable()) segThread_.join();
    SelOp sop = toOp(op);
    segThread_ = std::thread([this, region, sop]() {
        if (!seg_.ready()) seg_.prepare(sample_);
        Mask8 obj;
        seg_.objectSelect(obj, sample_, region, true);
        Mask8 cur = segBefore_;
        combineMask(cur, obj, sop);
        segResult_ = std::move(cur);
        segDone_ = true;
    });
    bump();
}

void Engine::finishAsyncSelection() {
    if (!segBusy_ || !segDone_) return;
    if (segThread_.joinable()) segThread_.join();
    segBusy_ = false; segDone_ = false;
    applySelection(segResult_, segName_.c_str());
}

void Engine::selectAll() {
    if (!W_ || segBusy_) return;
    Mask8 m; m.reset(W_, H_, 255);
    applySelection(m, "Selecionar tudo");
}

void Engine::deselect() {
    if (!hasSel_ || segBusy_) return;
    applySelection(Mask8(), "Desmarcar");
}

void Engine::invertSelection() {
    if (!W_ || segBusy_) return;
    Mask8 m;
    if (hasSel_) { m = sel_; for (auto& v : m.v) v = 255 - v; }
    else m.reset(W_, H_, 255);
    applySelection(m, "Inverter seleção");
}

void Engine::refineSelection(const RefineParams& p) {
    if (!hasSel_ || segBusy_) return;
    ensureSample(false);
    Mask8 m = sel_;
    refineEdge(m, sample_, p);
    applySelection(m, "Refinar borda");
}

// ======================================================================= edição do documento

void Engine::layerViaCopy(bool cut) {
    if (active_ < 0 || previewOn_ || segBusy_) return;
    auto comp = std::make_unique<HistEntry>(HistEntry::Compound, cut ? "Camada via recorte" : "Camada via cópia");
    Layer nl; nl.id = nextId_++;
    nl.name = layers_[active_].name + (cut ? " recorte" : " cópia");
    nl.tex = makeLayerTex(nullptr);
    bindTarget(nl.tex, W_, H_);
    glDisable(GL_BLEND);
    glUseProgram(pCopy_);
    bindTex(0, layers_[active_].tex, pCopy_, "uSrc");
    bindTex(1, selOrWhite(), pCopy_, "uMask");
    glUniformMatrix3fv(glGetUniformLocation(pCopy_, "uXform"), 1, GL_FALSE, IDENT3);
    drawFS();
    if (cut && hasSel_) {
        int x0, y0, x1, y1; selectionBounds(x0, y0, x1, y1);
        auto pe = std::make_unique<HistEntry>(HistEntry::Pixels, "");
        pe->layerId = layers_[active_].id; pe->x = x0; pe->y = y0; pe->w = x1 - x0; pe->h = y1 - y0;
        readTex(layers_[active_].tex, x0, y0, pe->w, pe->h, pe->before);
        bindTarget(preview_, W_, H_);
        glUseProgram(pFill_);
        bindTex(0, layers_[active_].tex, pFill_, "uLayer");
        bindTex(1, selTex_, pFill_, "uSel");
        bindTex(2, white1_, pFill_, "uRegion");
        glUniform1i(glGetUniformLocation(pFill_, "uMode"), 1);
        setFillCommon(pFill_, W_, H_);
        drawFS();
        std::swap(layers_[active_].tex, preview_);
        readTex(layers_[active_].tex, x0, y0, pe->w, pe->h, pe->after);
        comp->parts.push_back(std::move(pe));
    }
    auto before = metas(); int ab = activeLayerId();
    layers_.insert(layers_.begin() + active_ + 1, nl);
    active_++;
    auto se = std::make_unique<HistEntry>(HistEntry::Structure, "");
    se->metasBefore = before; se->activeBefore = ab;
    se->metasAfter = metas(); se->activeAfter = activeLayerId();
    std::vector<uint8_t> px; readTex(nl.tex, 0, 0, W_, H_, px);
    se->store.push_back({nl.id, std::move(px)});
    comp->parts.push_back(std::move(se));
    push(std::move(comp));
    layerChanged();
}

std::unique_ptr<HistEntry> Engine::snapshotDoc() {
    auto e = std::make_unique<HistEntry>(HistEntry::Doc, "");
    e->dw = W_; e->dh = H_;
    e->docMetas = metas(); e->docActive = activeLayerId();
    for (auto& l : layers_) { std::vector<uint8_t> px; readTex(l.tex, 0, 0, W_, H_, px); e->docPixels.push_back({l.id, std::move(px)}); }
    e->docHasSel = hasSel_;
    if (hasSel_) e->docSel = rleEncode(sel_);
    return e;
}

void Engine::restoreDoc(const HistEntry& e) {
    for (auto& l : layers_) glDeleteTextures(1, &l.tex);
    layers_.clear();
    GLuint t[] = {compA_, compB_, preview_, strokeTex_, tmp_, tmp2_, region_, selTex_};
    for (GLuint x : t) if (x) glDeleteTextures(1, &x);
    W_ = e.dw; H_ = e.dh;
    allocDocTextures();
    for (auto& m : e.docMetas) {
        Layer l; l.id = m.id; l.name = m.name; l.visible = m.visible; l.opacity = m.opacity; l.blend = m.blend; l.lockAlpha = m.lockAlpha;
        for (auto& p : e.docPixels) if (p.first == m.id) l.tex = makeLayerTex(p.second.data());
        if (!l.tex) l.tex = makeLayerTex(nullptr);
        layers_.push_back(l);
    }
    active_ = std::max(0, layerIndex(e.docActive));
    hasSel_ = e.docHasSel;
    if (hasSel_) rleDecode(e.docSel, sel_, W_, H_); else sel_ = Mask8();
    uploadSel();
}

void Engine::pushDocSnapshot(const char* name, std::unique_ptr<HistEntry> before) {
    auto e = std::make_unique<HistEntry>(HistEntry::Doc, name);
    e->docBefore = std::move(before);
    e->docAfter = snapshotDoc();
    push(std::move(e));
}

void Engine::cropToSelection() {
    int x0, y0, x1, y1;
    if (!selectionBounds(x0, y0, x1, y1)) return;
    cropRect(x0, y0, x1, y1);
}

void Engine::cropRect(int x0, int y0, int x1, int y1) {
    if (!W_ || previewOn_ || segBusy_) return;
    x0 = std::clamp(x0, 0, W_); x1 = std::clamp(x1, 0, W_); y0 = std::clamp(y0, 0, H_); y1 = std::clamp(y1, 0, H_);
    int nw = x1 - x0, nh = y1 - y0;
    if (nw < 1 || nh < 1 || (nw == W_ && nh == H_)) return;
    auto before = snapshotDoc();
    int oldW = W_, oldH = H_;
    std::vector<Layer> old = layers_;
    GLuint t[] = {compA_, compB_, preview_, strokeTex_, tmp_, tmp2_, region_, selTex_};
    for (GLuint x : t) if (x) glDeleteTextures(1, &x);
    W_ = nw; H_ = nh;
    allocDocTextures();
    // uv_src = uv_dst * (nw/oldW, nh/oldH) + (x0/oldW, y0/oldH)
    float xf[9] = {(float)nw / oldW, 0, 0, 0, (float)nh / oldH, 0, (float)x0 / oldW, (float)y0 / oldH, 1};
    for (auto& l : layers_) {
        GLuint nt = makeLayerTex(nullptr);
        bindTarget(nt, W_, H_);
        glDisable(GL_BLEND);
        glUseProgram(pCopy_);
        bindTex(0, l.tex, pCopy_, "uSrc");
        bindTex(1, white1_, pCopy_, "uMask");
        glUniformMatrix3fv(glGetUniformLocation(pCopy_, "uXform"), 1, GL_FALSE, xf);
        glActiveTexture(GL_TEXTURE0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        drawFS();
        glDeleteTextures(1, &l.tex);
        l.tex = nt;
    }
    sel_ = Mask8(); hasSel_ = false;
    pushDocSnapshot("Cortar", std::move(before));
    layerChanged();
}

void Engine::rotateCanvas(int q) {
    if (!W_ || previewOn_ || segBusy_) return;
    q = ((q % 4) + 4) % 4;
    if (!q) return;
    auto before = snapshotDoc();
    int nw = (q % 2) ? H_ : W_, nh = (q % 2) ? W_ : H_;
    GLuint t[] = {compA_, compB_, preview_, strokeTex_, tmp_, tmp2_, region_, selTex_};
    for (GLuint x : t) if (x) glDeleteTextures(1, &x);
    W_ = nw; H_ = nh;
    allocDocTextures();
    // giro horário de 90°: origem (su, sv) = (v, 1-u); 180°: (1-u, 1-v); 270°: (1-v, u)
    float xf[9];
    if (q == 1) { float m[9] = {0, -1, 0, 1, 0, 0, 0, 1, 1}; std::memcpy(xf, m, sizeof xf); }
    else if (q == 2) { float m[9] = {-1, 0, 0, 0, -1, 0, 1, 1, 1}; std::memcpy(xf, m, sizeof xf); }
    else { float m[9] = {0, 1, 0, -1, 0, 0, 1, 0, 1}; std::memcpy(xf, m, sizeof xf); }
    for (auto& l : layers_) {
        GLuint nt = makeLayerTex(nullptr);
        bindTarget(nt, W_, H_);
        glDisable(GL_BLEND);
        glUseProgram(pCopy_);
        bindTex(0, l.tex, pCopy_, "uSrc");
        bindTex(1, white1_, pCopy_, "uMask");
        glUniformMatrix3fv(glGetUniformLocation(pCopy_, "uXform"), 1, GL_FALSE, xf);
        drawFS();
        glDeleteTextures(1, &l.tex);
        l.tex = nt;
    }
    sel_ = Mask8(); hasSel_ = false;
    pushDocSnapshot(q == 1 ? "Girar 90° horário" : q == 2 ? "Girar 180°" : "Girar 90° anti-horário", std::move(before));
    layerChanged();
}

void Engine::flipCanvas(bool horizontal) {
    if (!W_ || previewOn_ || segBusy_) return;
    auto before = snapshotDoc();
    float xf[9];
    if (horizontal) { float m[9] = {-1, 0, 0, 0, 1, 0, 1, 0, 1}; std::memcpy(xf, m, sizeof xf); }
    else { float m[9] = {1, 0, 0, 0, -1, 0, 0, 1, 1}; std::memcpy(xf, m, sizeof xf); }
    for (auto& l : layers_) {
        bindTarget(preview_, W_, H_);
        glDisable(GL_BLEND);
        glUseProgram(pCopy_);
        bindTex(0, l.tex, pCopy_, "uSrc");
        bindTex(1, white1_, pCopy_, "uMask");
        glUniformMatrix3fv(glGetUniformLocation(pCopy_, "uXform"), 1, GL_FALSE, xf);
        drawFS();
        std::swap(l.tex, preview_);
    }
    sel_ = Mask8(); hasSel_ = false;
    pushDocSnapshot(horizontal ? "Espelhar na horizontal" : "Espelhar na vertical", std::move(before));
    layerChanged();
}

// ======================================================================= ajustes

void Engine::adjustBegin(int type) {
    if (active_ < 0 || previewOn_ || segBusy_) return;
    adjType_ = type;
    std::fill(std::begin(adjP_), std::end(adjP_), 0.f);
    if (type == 2) adjP_[2] = 1.f;                   // gama
    if (type == 4) { adjP_[1] = 1.f; adjP_[2] = 1.f; adjP_[5] = 1.f; }
    previewOn_ = true;
    renderAdjust(preview_);
}

void Engine::adjustSet(const float p[8], const float* curve) {
    if (adjType_ < 0) return;
    std::memcpy(adjP_, p, sizeof adjP_);
    if (curve) {
        uint8_t c[256];
        for (int i = 0; i < 256; ++i) c[i] = (uint8_t)std::lround(std::clamp(curve[i], 0.f, 1.f) * 255.f);
        glBindTexture(GL_TEXTURE_2D, curveTex_);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 256, 1, GL_RED, GL_UNSIGNED_BYTE, c);
    }
    renderAdjust(preview_);
}

void Engine::renderAdjust(GLuint dst) {
    GLuint src = layers_[active_].tex;
    glDisable(GL_BLEND);
    if (adjType_ == 5 || adjType_ == 6) {
        float sigma = adjType_ == 5 ? adjP_[0] : adjP_[1];
        glUseProgram(pBlur_);
        bindTarget(tmp2_, W_, H_);
        bindTex(0, src, pBlur_, "uSrc");
        glUniform2f(glGetUniformLocation(pBlur_, "uDir"), 1.f / W_, 0.f);
        glUniform1f(glGetUniformLocation(pBlur_, "uSigma"), sigma);
        drawFS();
        bindTarget(tmp_, W_, H_);
        bindTex(0, tmp2_, pBlur_, "uSrc");
        glUniform2f(glGetUniformLocation(pBlur_, "uDir"), 0.f, 1.f / H_);
        drawFS();
    }
    bindTarget(dst, W_, H_);
    glUseProgram(pAdjust_);
    bindTex(0, src, pAdjust_, "uSrc");
    bindTex(1, tmp_, pAdjust_, "uBlur");
    bindTex(2, selOrWhite(), pAdjust_, "uSel");
    bindTex(3, curveTex_, pAdjust_, "uCurve");
    glUniform1i(glGetUniformLocation(pAdjust_, "uType"), adjType_);
    glUniform4f(glGetUniformLocation(pAdjust_, "uP"), adjP_[0], adjP_[1], adjP_[2], adjP_[3]);
    glUniform4f(glGetUniformLocation(pAdjust_, "uQ"), adjP_[4], adjP_[5], adjP_[6], adjP_[7]);
    drawFS();
    docDirty_ = true;
}

static const char* adjName(int t) {
    static const char* n[] = {"Brilho/Contraste", "Matiz/Saturação", "Exposição", "Temperatura e vibração", "Níveis",
                              "Desfoque gaussiano", "Nitidez", "Preto e branco", "Inverter", "Curvas"};
    return t >= 0 && t < 10 ? n[t] : "Ajuste";
}

void Engine::adjustCommit() {
    if (adjType_ < 0) return;
    int x0 = 0, y0 = 0, x1 = W_, y1 = H_;
    if (hasSel_) selectionBounds(x0, y0, x1, y1);
    const char* n = adjName(adjType_);
    adjType_ = -1;
    commitPreviewToLayer(n, x0, y0, x1, y1);
}

void Engine::adjustCancel() {
    if (adjType_ < 0) return;
    adjType_ = -1; previewOn_ = false; docDirty_ = true; bump();
}

// ======================================================================= histórico

void Engine::push(std::unique_ptr<HistEntry> e) {
    // descarta o que estava "à frente" (refazer)
    while ((int)hist_.size() > histPos_) { histBytes_ -= hist_.back()->bytes; hist_.pop_back(); }
    e->bytes = e->size();
    histBytes_ += e->bytes;
    hist_.push_back(std::move(e));
    histPos_ = (int)hist_.size();
    // limite: mantém o primeiro (Abrir) e remove os mais antigos depois dele
    while (hist_.size() > 2 && (histBytes_ > HIST_BUDGET || (int)hist_.size() > HIST_MAX)) {
        histBytes_ -= hist_[1]->bytes;
        hist_[0]->name = "Estado anterior";
        hist_.erase(hist_.begin() + 1);
        histPos_--;
    }
    bump();
}

void Engine::pushStructure(const char* name, const std::vector<LayerMeta>& before, int ab,
                           const std::vector<int>& removedIds, const std::vector<std::vector<uint8_t>>& removedPx) {
    auto e = std::make_unique<HistEntry>(HistEntry::Structure, name);
    e->metasBefore = before; e->activeBefore = ab;
    e->metasAfter = metas(); e->activeAfter = activeLayerId();
    for (size_t i = 0; i < removedIds.size(); ++i) e->store.push_back({removedIds[i], removedPx[i]});
    // camadas novas: guarda os pixels para poder refazer
    for (auto& m : e->metasAfter) {
        bool existed = false;
        for (auto& b : before) if (b.id == m.id) existed = true;
        if (!existed) {
            std::vector<uint8_t> px; readTex(layers_[layerIndex(m.id)].tex, 0, 0, W_, H_, px);
            e->store.push_back({m.id, std::move(px)});
        }
    }
    push(std::move(e));
}

void Engine::applyMetas(const std::vector<LayerMeta>& target, int activeId,
                        const std::vector<std::pair<int, std::vector<uint8_t>>>& store) {
    std::vector<Layer> nl;
    for (auto& m : target) {
        int i = layerIndex(m.id);
        Layer l;
        if (i >= 0) { l = layers_[i]; layers_[i].tex = 0; }
        else {
            l.id = m.id;
            for (auto& s : store) if (s.first == m.id) l.tex = makeLayerTex(s.second.data());
            if (!l.tex) l.tex = makeLayerTex(nullptr);
        }
        l.name = m.name; l.visible = m.visible; l.opacity = m.opacity; l.blend = m.blend; l.lockAlpha = m.lockAlpha;
        nl.push_back(l);
    }
    for (auto& l : layers_) if (l.tex) glDeleteTextures(1, &l.tex);
    layers_ = std::move(nl);
    active_ = std::max(0, layerIndex(activeId));
}

void Engine::applyEntry(HistEntry& e, bool undo) {
    switch (e.kind) {
        case HistEntry::Pixels: {
            int i = layerIndex(e.layerId);
            if (i >= 0 && e.w > 0 && e.h > 0) writeTex(layers_[i].tex, e.x, e.y, e.w, e.h, (undo ? e.before : e.after).data());
            break;
        }
        case HistEntry::Selection: {
            bool has = undo ? e.hadSel : e.hasSel;
            if (has) rleDecode(undo ? e.selBefore : e.selAfter, sel_, W_, H_); else sel_ = Mask8();
            hasSel_ = has;
            uploadSel();
            break;
        }
        case HistEntry::Structure:
            applyMetas(undo ? e.metasBefore : e.metasAfter, undo ? e.activeBefore : e.activeAfter, e.store);
            break;
        case HistEntry::Doc:
            restoreDoc(undo ? *e.docBefore : *e.docAfter);
            break;
        case HistEntry::Compound:
            if (undo) for (auto it = e.parts.rbegin(); it != e.parts.rend(); ++it) applyEntry(**it, true);
            else for (auto& p : e.parts) applyEntry(*p, false);
            break;
        case HistEntry::Marker: break;
    }
}

bool Engine::undo() {
    if (histPos_ <= 1 || previewOn_ || segBusy_) return false;
    applyEntry(*hist_[histPos_ - 1], true);
    histPos_--;
    layerChanged();
    return true;
}

bool Engine::redo() {
    if (histPos_ >= (int)hist_.size() || previewOn_ || segBusy_) return false;
    applyEntry(*hist_[histPos_], false);
    histPos_++;
    layerChanged();
    return true;
}

void Engine::jumpHistory(int index) {
    int target = std::clamp(index + 1, 1, (int)hist_.size());
    while (histPos_ > target && undo()) {}
    while (histPos_ < target && redo()) {}
}

} // namespace bp
