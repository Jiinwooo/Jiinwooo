// Teste de host do motor GL: roda os mesmos shaders e o mesmo código do APK sobre OpenGL ES 3.0
// por software (Mesa llvmpipe). Gera imagens em out/ para conferência visual.
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <GLES3/gl3.h>
#include "../app/src/main/cpp/engine.h"
#include <cmath>
#include <cstring>
#include <cstdio>
#include <fstream>
#include <sstream>
#include <thread>
#include <chrono>
using namespace bp;

static int fails = 0;
#define CHECK(c, ...) do { if (!(c)) { fails++; printf("FALHA: " __VA_ARGS__); printf("\n"); } else { printf("ok   " __VA_ARGS__); printf("\n"); } } while (0)

static std::string root;
static std::string load(const std::string& n) {
    std::string p = n.rfind("shaders/", 0) == 0 ? root + "/app/src/main/assets/" + n : root + "/../telar/filters/" + n;
    std::ifstream f(p); std::stringstream s; s << f.rdbuf(); return s.str();
}
static void writePpm(const char* f, const std::vector<uint8_t>& rgba, int w, int h) {
    FILE* fp = fopen(f, "wb"); fprintf(fp, "P6 %d %d 255\n", w, h);
    for (size_t i = 0; i < (size_t)w * h; ++i) {
        int a = rgba[i * 4 + 3];
        for (int c = 0; c < 3; ++c) { int v = rgba[i * 4 + c] + (255 - a); fputc(std::min(255, v), fp); }  // sobre branco
    }
    fclose(fp);
}
static void px(const std::vector<uint8_t>& im, int w, int x, int y, int* o) { for (int c = 0; c < 4; ++c) o[c] = im[((size_t)y * w + x) * 4 + c]; }

int main(int argc, char** argv) {
    root = argc > 1 ? argv[1] : "..";
    auto getPD = (PFNEGLGETPLATFORMDISPLAYEXTPROC)eglGetProcAddress("eglGetPlatformDisplayEXT");
    // Mesa sem janela (llvmpipe): EGL_PLATFORM_SURFACELESS_MESA
    EGLDisplay dpy = getPD ? getPD(0x31DD, (void*)EGL_DEFAULT_DISPLAY, nullptr) : eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (!eglInitialize(dpy, nullptr, nullptr)) { printf("eglInitialize falhou 0x%x\n", eglGetError()); return 2; }
    const EGLint cattr[] = {EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT, EGL_SURFACE_TYPE, EGL_PBUFFER_BIT, EGL_RED_SIZE, 8, EGL_GREEN_SIZE, 8, EGL_BLUE_SIZE, 8, EGL_ALPHA_SIZE, 8, EGL_NONE};
    EGLConfig cfg; EGLint n;
    eglChooseConfig(dpy, cattr, &cfg, 1, &n);
    const int SW = 900, SH = 600;
    const EGLint sattr[] = {EGL_WIDTH, SW, EGL_HEIGHT, SH, EGL_NONE};
    EGLSurface surf = eglCreatePbufferSurface(dpy, cfg, sattr);
    const EGLint ctxa[] = {EGL_CONTEXT_CLIENT_VERSION, 3, EGL_NONE};
    EGLContext ctx = eglCreateContext(dpy, cfg, EGL_NO_CONTEXT, ctxa);
    if (!eglMakeCurrent(dpy, surf, surf, ctx)) { printf("makeCurrent falhou 0x%x\n", eglGetError()); return 2; }

    Engine e(load);
    CHECK(e.initGL(), "shaders compilam e linkam (%s)", e.glInfo().c_str());
    e.surfaceChanged(SW, SH);

    // foto sintética 1200x800: céu, sol e um "objeto" laranja
    const int W = 1200, H = 800;
    std::vector<uint8_t> img((size_t)W * H * 4);
    for (int y = 0; y < H; ++y) for (int x = 0; x < W; ++x) {
        uint8_t* p = &img[((size_t)y * W + x) * 4];
        float t = (float)y / H;
        float r = 60 + 150 * t, g = 90 + 80 * t, b = 200 - 90 * t;
        float d = std::hypot(x - 700.f, y - 450.f);
        float a = std::clamp(180 - d + 0.5f, 0.f, 1.f);
        p[0] = (uint8_t)(r * (1 - a) + 240 * a); p[1] = (uint8_t)(g * (1 - a) + 150 * a); p[2] = (uint8_t)(b * (1 - a) + 30 * a); p[3] = 255;
    }
    CHECK(e.openImage(img.data(), W, H, "teste"), "abrir imagem %dx%d", W, H);
    std::vector<uint8_t> out;
    e.exportComposite(out);
    int c[4]; px(out, W, 10, 10, c);
    CHECK(std::abs(c[0] - img[(10 * W + 10) * 4]) <= 1 && c[3] == 255, "exportar devolve a imagem (%d,%d,%d,%d)", c[0], c[1], c[2], c[3]);

    // camada nova + pincel vermelho
    e.addLayer();
    float red[4] = {1, 0, 0, 1};
    e.strokeBegin(Tool::Brush, 100, 100, 1, 40, 1.0f, 1.0f, red, false, false);
    for (int i = 1; i <= 20; ++i) e.strokeMove(100 + i * 20.f, 100 + i * 5.f, 1);
    e.strokeEnd();
    e.exportComposite(out);
    px(out, W, 300, 150, c);
    CHECK(c[0] > 240 && c[1] < 15 && c[2] < 15, "pincel pinta vermelho (%d,%d,%d)", c[0], c[1], c[2]);
    px(out, W, 300, 300, c);
    CHECK(!(c[0] > 240 && c[1] < 15), "pincel não vaza para longe");

    // modo de mesclagem Multiplicação na camada do pincel: vermelho × céu
    e.setLayerBlend(3);
    e.exportComposite(out);
    px(out, W, 300, 150, c);
    int bgR = img[(150 * W + 300) * 4];
    CHECK(std::abs(c[0] - bgR) <= 2 && c[1] < 5, "Multiplicação: vermelho × fundo = (%d,%d,%d), esperado R≈%d", c[0], c[1], c[2], bgR);
    e.setLayerOpacity(0.5f, true);

    // borracha
    e.strokeBegin(Tool::Eraser, 300, 150, 1, 30, 1, 1, red, false, false);
    e.strokeEnd();
    e.exportComposite(out);
    px(out, W, 300, 150, c);
    CHECK(std::abs(c[0] - bgR) <= 2 && std::abs(c[1] - img[(150 * W + 300) * 4 + 1]) <= 2, "borracha volta ao fundo");

    // desfazer 3x (borracha, opacidade, mesclagem) → vermelho multiplicado de volta? volta a Normal vermelho
    e.undo(); e.undo(); e.undo();
    e.exportComposite(out);
    px(out, W, 300, 150, c);
    CHECK(c[0] > 240 && c[1] < 15, "desfazer 3 passos restaura pincel vermelho normal (%d,%d,%d)", c[0], c[1], c[2]);
    e.redo();
    e.exportComposite(out);
    px(out, W, 300, 150, c);
    CHECK(std::abs(c[0] - bgR) <= 2, "refazer reaplica Multiplicação");

    // seleção de objeto no fundo (assíncrona) ao redor do disco
    e.setActiveLayer(1);
    std::vector<Pt> rect = {{450, 200}, {950, 200}, {950, 700}, {450, 700}};
    e.objectSelect(rect, false, 0);
    for (int i = 0; i < 400 && e.selectionBusy(); ++i) { e.render(0); std::this_thread::sleep_for(std::chrono::milliseconds(20)); }
    e.render(0);
    int x0, y0, x1, y1;
    bool hs = e.selectionBounds(x0, y0, x1, y1);
    CHECK(hs && std::abs(x0 - 520) <= 4 && std::abs(x1 - 880) <= 4 && std::abs(y0 - 270) <= 4 && std::abs(y1 - 630) <= 4,
          "seleção de objeto acha o disco: [%d,%d)-[%d,%d) esperado ~[520,880)x[270,630)", x0, x1, y0, y1);

    // ajuste Matiz/Saturação só dentro da seleção: laranja → azulado
    e.adjustBegin(1);
    float p[8] = {180, 0, 0, 0, 0, 0, 0, 0};
    e.adjustSet(p, nullptr);
    e.adjustCommit();
    e.exportComposite(out);
    px(out, W, 700, 450, c);
    CHECK(c[2] > c[0], "matiz 180° dentro da seleção inverte o laranja (%d,%d,%d)", c[0], c[1], c[2]);
    px(out, W, 100, 700, c);
    int o[4]; px(img, W, 100, 700, o);
    CHECK(std::abs(c[0] - o[0]) <= 1 && std::abs(c[2] - o[2]) <= 1, "fora da seleção nada muda");

    // camada via cópia + mover
    e.layerViaCopy(false);
    e.moveBegin(); e.moveUpdate(-300, 0); e.moveEnd();
    e.exportComposite(out);
    px(out, W, 400, 450, c);
    CHECK(c[2] > c[0], "camada via cópia movida 300 px para a esquerda (%d,%d,%d)", c[0], c[1], c[2]);

    // desfoque e nitidez compilam e rodam
    e.deselect();
    e.adjustBegin(5); float pb[8] = {6, 0, 0, 0, 0, 0, 0, 0}; e.adjustSet(pb, nullptr); e.adjustCommit();
    e.adjustBegin(6); float ps[8] = {1.0f, 2.0f, 0, 0, 0, 0, 0, 0}; e.adjustSet(ps, nullptr); e.adjustCommit();

    // degradê numa camada nova
    e.addLayer();
    float a0[4] = {0, 0, 0, 1}, a1[4] = {1, 1, 1, 0};
    e.gradientPreview(0, 0, 0, 200, a0, a1, false); e.gradientCommit();
    e.setLayerOpacity(0.6f, true);

    // conta-gotas
    uint32_t col = e.sampleColor(700, 450);
    printf("     conta-gotas em (700,450): #%08X\n", col);

    // tela
    float view[6] = {0.7f, 0, 0, 0.7f, 30, 20};
    e.setView(view);
    e.render(0.5f);
    std::vector<uint8_t> scr((size_t)SW * SH * 4);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glReadPixels(0, 0, SW, SH, GL_RGBA, GL_UNSIGNED_BYTE, scr.data());
    std::vector<uint8_t> flip(scr.size());
    for (int y = 0; y < SH; ++y) memcpy(&flip[(size_t)y * SW * 4], &scr[(size_t)(SH - 1 - y) * SW * 4], SW * 4);
    writePpm("out/tela.ppm", flip, SW, SH);

    e.exportComposite(out);
    writePpm("out/documento.ppm", out, W, H);

    // girar e cortar, depois desfazer
    e.rotateCanvas(1);
    CHECK(e.docW() == H && e.docH() == W, "girar 90° troca dimensões (%dx%d)", e.docW(), e.docH());
    e.cropRect(100, 100, 500, 400);
    CHECK(e.docW() == 400 && e.docH() == 300, "cortar 400x300");
    e.undo(); e.undo();
    CHECK(e.docW() == W && e.docH() == H, "desfazer corte e giro");
    e.flatten();
    printf("     histórico: %s\n", e.historyJson().c_str());
    printf("     camadas: %s\n", e.layersJson().c_str());
    GLenum err = glGetError();
    CHECK(err == GL_NO_ERROR, "sem erro de GL (0x%x)", err);
    printf("%s\n", fails ? "FALHOU" : "MOTOR OK");
    return fails ? 1 : 0;
}
