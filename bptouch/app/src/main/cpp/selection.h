// SPDX-License-Identifier: MIT OR Apache-2.0
// BP Touch — algoritmos de seleção na CPU.
//
// Tudo trabalha sobre uma máscara de 8 bits do tamanho do documento (0 = fora, 255 = dentro,
// valores intermediários = borda suave). Métodos usados (todos de literatura pública):
//   * Letreiro / Laço: rasterização de polígono com cobertura por subamostragem (antisserrilhado).
//   * Varinha mágica / Lata de tinta: preenchimento por semente com tolerância.
//   * Seleção rápida: corte de grafo com modelos de cor GMM + prior de distância ao traço,
//     em resolução de trabalho, seguido de refino de borda guiado (sem rebarba).
//   * Seleção de objeto: GrabCut (Rother et al. 2004) iterativo dentro do retângulo/laço.
//   * Refinar borda: filtro guiado rápido (He et al. 2010; He & Sun 2015) usando a foto
//     como guia, com contraste, deslocamento e difusão.
#pragma once
#include <cstdint>
#include <vector>

namespace bp {

struct Image8 {          // RGBA 8 bits, pré-multiplicado, linhas de cima para baixo
    int w = 0, h = 0;
    std::vector<uint8_t> px;
    bool empty() const { return w == 0 || h == 0; }
};

struct Mask8 {
    int w = 0, h = 0;
    std::vector<uint8_t> v;
    void reset(int W, int H, uint8_t val = 0) { w = W; h = H; v.assign((size_t)W * H, val); }
    bool empty() const { return w == 0 || h == 0; }
};

enum class SelOp { Replace = 0, Add = 1, Subtract = 2, Intersect = 3 };

struct Pt { float x, y; };

// Combina "novo" em "atual" segundo a operação (substituir, adicionar, subtrair, interseção).
void combineMask(Mask8& cur, const Mask8& add, SelOp op);
bool maskIsEmpty(const Mask8& m);
bool maskBounds(const Mask8& m, int& x0, int& y0, int& x1, int& y1);   // [x0,x1) [y0,y1)

// Polígono (fechado automaticamente) com antisserrilhado opcional.
void rasterPolygon(Mask8& out, int w, int h, const std::vector<Pt>& pts, bool antialias);
void ellipsePolygon(std::vector<Pt>& out, float x0, float y0, float x1, float y1);

// Difusão gaussiana aproximada (três caixas) de raio r pixels.
void featherMask(Mask8& m, float radius);

// Preenchimento por semente: tolerância 0..255 por canal; contíguo ou global.
void magicWand(Mask8& out, const Image8& img, int sx, int sy, int tolerance, bool contiguous, bool antialias);

// Seleção rápida: pinceladas em coordenadas do documento.
struct QuickSelectParams {
    std::vector<Pt> stroke;
    float brushRadius = 20.f;
    bool subtract = false;
    bool refineEdges = true;     // filtro guiado na resolução cheia
};

// Guarda a imagem reduzida e os pesos do grafo entre uma pincelada e outra.
// Precisa ser refeito (prepare) quando os pixels da camada mudam.
class Segmenter {
public:
    void prepare(const Image8& img, int workMax = 900);
    void invalidate() { ready_ = false; }
    bool ready() const { return ready_; }
    void quickSelect(Mask8& sel, const Image8& img, const QuickSelectParams& p);
    void objectSelect(Mask8& out, const Image8& img, const std::vector<Pt>& region, bool refineEdges);

    // usados pelo refino
    int ww = 0, wh = 0;          // tamanho de trabalho
    float scale = 1.f;           // pixels do documento por pixel de trabalho
    std::vector<float> rgb;      // imagem de trabalho, 0..1, intercalada
private:
    bool ready_ = false;
    float beta_ = 0.f;
    std::vector<int32_t> wR_, wD_, wDR_, wDL_;   // pesos das arestas (direita, baixo, diagonais)
    void buildEdges();
    void cut(const std::vector<float>& costFg, const std::vector<float>& costBg,
             const std::vector<uint8_t>& hard, std::vector<uint8_t>& label);
};

// Refinar borda da seleção atual usando a imagem como guia.
struct RefineParams {
    float radius = 6.f;       // raio do filtro guiado, em pixels do documento
    float smooth = 0.f;       // 0..1: arredonda a borda antes do filtro
    float contrast = 0.35f;   // 0..1: endurece a transição
    float shift = 0.f;        // -1..1: contrai (<0) ou expande (>0) a borda
    float feather = 0.f;      // pixels de difusão final
};
void refineEdge(Mask8& m, const Image8& img, const RefineParams& p);

// Filtro guiado rápido: p (tamanho de trabalho, 0..1) → alfa na resolução cheia, só dentro de
// [rx0,rx1)×[ry0,ry1). Guia = a própria foto.
void guidedUpsample(const Segmenter& seg, const Image8& img, const std::vector<float>& p,
                    int radius, float eps, int rx0, int ry0, int rx1, int ry1,
                    std::vector<float>& out /* (rx1-rx0)*(ry1-ry0) */);

// Compressão simples (RLE) para guardar seleções no histórico.
std::vector<uint8_t> rleEncode(const Mask8& m);
void rleDecode(const std::vector<uint8_t>& data, Mask8& m, int w, int h);

} // namespace bp
