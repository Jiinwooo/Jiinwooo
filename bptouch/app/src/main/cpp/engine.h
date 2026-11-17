// SPDX-License-Identifier: MIT OR Apache-2.0
// BP Touch — motor de edição (OpenGL ES 3.0).
// Todas as chamadas acontecem na thread do GL. A UI manda comandos; o motor guarda documento,
// camadas, seleção e histórico.
#pragma once
#include <GLES3/gl3.h>
#include <atomic>
#include <cstdint>
#include <functional>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <vector>
#include "selection.h"

namespace bp {

struct Layer {
    int id = 0;
    std::string name;
    GLuint tex = 0;
    bool visible = true;
    float opacity = 1.f;
    int blend = 0;
    bool lockAlpha = false;
};

struct LayerMeta {
    int id; std::string name; bool visible; float opacity; int blend; bool lockAlpha;
};

struct HistEntry;

enum class Tool { Idle, Brush, Eraser, Clone, Move, Gradient };

class Engine {
public:
    using Loader = std::function<std::string(const std::string&)>;
    explicit Engine(Loader loader);
    ~Engine();

    bool initGL();                       // compila shaders; chamar com contexto ativo
    void releaseGL();                    // contexto perdido
    void surfaceChanged(int w, int h);
    // desenha um quadro; devolve true se precisa de animação contínua (formigas)
    bool render(float timeSec);
    void setView(const float m[6]);      // documento → tela: [a b c d tx ty]
    std::string glInfo() const { return glInfo_; }

    // documento
    bool newDocument(int w, int h, bool whiteBackground);
    bool openImage(const uint8_t* rgbaPremul, int w, int h, const std::string& name);
    bool addImageLayer(const uint8_t* rgbaPremul, int w, int h, const std::string& name);
    bool exportComposite(std::vector<uint8_t>& out);   // RGBA pré-multiplicado, w*h*4
    int docW() const { return W_; }
    int docH() const { return H_; }
    int maxTextureSize() const { return maxTex_; }

    // camadas
    std::string layersJson() const;
    int activeLayerId() const;
    void setActiveLayer(int id);
    void addLayer();
    void duplicateLayer();
    void deleteLayer();
    void moveLayer(int delta);
    void setLayerVisible(int id, bool v);
    void setLayerOpacity(float o, bool final);
    void setLayerBlend(int mode);
    void setLayerLockAlpha(bool v);
    void renameLayer(int id, const std::string& name);
    void mergeDown();
    void flatten();
    bool layerThumbnail(int id, int tw, int th, std::vector<uint8_t>& out);

    // pincel / borracha / carimbo
    void strokeBegin(Tool t, float x, float y, float pressure, float size, float hardness, float opacity,
                     const float rgba[4], bool pressureSize, bool pressureOpacity);
    void strokeMove(float x, float y, float pressure);
    void strokeEnd();
    void strokeCancel();
    void moveCancel();
    void setCloneSource(float x, float y) { cloneSrcX_ = x; cloneSrcY_ = y; cloneSrcSet_ = true; }
    bool cloneSourceSet() const { return cloneSrcSet_; }

    // mover
    void moveBegin();
    void moveUpdate(float dx, float dy);
    void moveEnd();

    // degradê, preencher, lata de tinta, conta-gotas
    void gradientPreview(float x0, float y0, float x1, float y1, const float a[4], const float b[4], bool radial);
    void gradientCommit();
    void gradientCancel();
    void fillSelection(const float rgba[4]);
    void clearSelectionPixels();
    void bucketFill(float x, float y, int tolerance, bool contiguous, const float rgba[4]);
    uint32_t sampleColor(float x, float y);   // 0xAARRGGBB não pré-multiplicada

    // seleção
    void selectPolygon(const std::vector<Pt>& pts, int op, float feather, bool aa, const char* name);
    void selectEllipse(float x0, float y0, float x1, float y1, int op, float feather, bool aa);
    void magicWand(float x, float y, int tolerance, bool contiguous, bool aa, bool allLayers, int op);
    void quickSelect(const std::vector<Pt>& stroke, float radius, bool subtract, bool allLayers);
    void objectSelect(const std::vector<Pt>& region, bool allLayers, int op);
    bool selectionBusy() const { return segBusy_.load(); }
    void selectAll();
    void deselect();
    void invertSelection();
    void refineSelection(const RefineParams& p);
    bool hasSelection() const { return hasSel_; }
    bool selectionBounds(int& x0, int& y0, int& x1, int& y1) const;

    // edição do documento
    void layerViaCopy(bool cut);
    void cropToSelection();
    void cropRect(int x0, int y0, int x1, int y1);
    void rotateCanvas(int quarterTurns);
    void flipCanvas(bool horizontal);

    // ajustes: begin → set (prévia) → commit/cancel
    void adjustBegin(int type);
    void adjustSet(const float p[8], const float* curve256);
    void adjustCommit();
    void adjustCancel();
    bool adjusting() const { return adjType_ >= 0; }

    // histórico
    bool undo();
    bool redo();
    void jumpHistory(int index);   // estado após o passo index (0 = abrir)
    std::string historyJson() const;

    // muda sempre que algo visível pela UI muda (camadas, histórico, seleção)
    uint32_t stateVersion() const { return stateVer_.load(); }

private:
    Loader loader_;
    std::string glInfo_;
    int maxTex_ = 4096;
    bool halfFloatRT_ = false;
    int screenW_ = 1, screenH_ = 1;
    float view_[6] = {1, 0, 0, 1, 0, 0};

    // programas
    GLuint pBlend_ = 0, pCopy_ = 0, pThumb_ = 0, pDab_ = 0, pStroke_ = 0, pMove_ = 0, pFill_ = 0,
           pBlur_ = 0, pAdjust_ = 0, pDisplay_ = 0;
    GLuint vao_ = 0, dabVbo_ = 0, fbo_ = 0, curveTex_ = 0;
    GLuint white1_ = 0, clear1_ = 0;

    // documento
    int W_ = 0, H_ = 0;
    std::vector<Layer> layers_;     // 0 = de baixo
    int active_ = -1;               // índice
    int nextId_ = 1;
    GLuint compA_ = 0, compB_ = 0, compOut_ = 0;
    GLuint preview_ = 0, strokeTex_ = 0, tmp_ = 0, tmp2_ = 0, region_ = 0;
    bool docDirty_ = true, previewOn_ = false;

    // seleção
    Mask8 sel_;
    bool hasSel_ = false;
    GLuint selTex_ = 0;
    Segmenter seg_;
    Image8 sample_;
    bool sampleValid_ = false, sampleAll_ = false;
    std::thread segThread_;
    std::atomic<bool> segBusy_{false}, segDone_{false};
    Mask8 segResult_, segBefore_;
    std::string segName_;

    // traço
    Tool tool_ = Tool::Idle;
    std::vector<float> dabs_;
    float lastX_ = 0, lastY_ = 0, lastP_ = 1, carry_ = 0;
    float bSize_ = 20, bHard_ = 0.8f, bOpacity_ = 1;
    float bColor_[4] = {0, 0, 0, 1};
    bool bPrSize_ = true, bPrOpacity_ = false;
    int sx0_, sy0_, sx1_, sy1_;      // retângulo sujo do traço
    float cloneSrcX_ = 0, cloneSrcY_ = 0, cloneOffX_ = 0, cloneOffY_ = 0;
    bool cloneSrcSet_ = false, cloneAligned_ = false;
    float moveDx_ = 0, moveDy_ = 0;
    float gradP_[4]; float gradA_[4], gradB_[4]; bool gradRadial_ = false;

    // ajuste
    int adjType_ = -1;
    float adjP_[8] = {0};

    // histórico
    std::vector<std::unique_ptr<HistEntry>> hist_;
    int histPos_ = 0;
    size_t histBytes_ = 0;
    std::atomic<uint32_t> stateVer_{1};

    // utilidades GL
    GLuint program(const std::string& vs, const std::string& fs);
    GLuint makeTex(int w, int h, GLenum ifmt, GLenum fmt, GLenum type, const void* data, bool linear);
    GLuint makeLayerTex(const uint8_t* data);
    void bindTarget(GLuint tex, int w, int h);
    void drawFS();
    void bindTex(int unit, GLuint tex, GLuint prog, const char* name);
    void readTex(GLuint tex, int x, int y, int w, int h, std::vector<uint8_t>& out);
    void writeTex(GLuint tex, int x, int y, int w, int h, const uint8_t* data);
    void copyTex(GLuint src, GLuint dst);
    void destroyDoc();
    void allocDocTextures();

    // pipeline
    void composite();
    void renderStrokePreview();
    void stampTo(float x, float y, float p);
    void flushDabs();
    void renderAdjust(GLuint dst);
    void uploadSel();
    void ensureSample(bool allLayers);
    void applySelection(const Mask8& m, const char* name);
    void finishAsyncSelection();
    GLuint selOrWhite() const { return hasSel_ ? selTex_ : white1_; }
    void commitPreviewToLayer(const char* name, int x0, int y0, int x1, int y1);
    void layerChanged();

    // histórico
    void push(std::unique_ptr<HistEntry> e);
    std::vector<LayerMeta> metas() const;
    void applyMetas(const std::vector<LayerMeta>& m, int activeId,
                    const std::vector<std::pair<int, std::vector<uint8_t>>>& store);
    void pushStructure(const char* name, const std::vector<LayerMeta>& before, int activeBefore,
                       const std::vector<int>& removedIds, const std::vector<std::vector<uint8_t>>& removedPx);
    void pushDocSnapshot(const char* name, std::unique_ptr<HistEntry> before);
    std::unique_ptr<HistEntry> snapshotDoc();
    void restoreDoc(const HistEntry& e);
    void applyEntry(HistEntry& e, bool undo);
    int layerIndex(int id) const;
    void bump() { stateVer_.fetch_add(1); }

    friend struct HistEntry;
};

} // namespace bp
