// Testes de host do motor de seleção (sem GPU).
#include "../app/src/main/cpp/maxflow.h"
#include "../app/src/main/cpp/selection.h"
#include <cstdio>
#include <cmath>
#include <random>
#include <chrono>
#include <queue>
#include <functional>
using namespace bp;

// referência: Edmonds-Karp em matriz densa
static long long ek(int n, std::vector<std::vector<long long>> c, int s, int t, std::vector<int>& side) {
    long long f = 0;
    for (;;) {
        std::vector<int> par(n, -1); par[s] = s; std::queue<int> q; q.push(s);
        while (!q.empty() && par[t] < 0) { int u = q.front(); q.pop(); for (int v = 0; v < n; ++v) if (par[v] < 0 && c[u][v] > 0) { par[v] = u; q.push(v); } }
        if (par[t] < 0) break;
        long long b = 1LL << 60; for (int v = t; v != s; v = par[v]) b = std::min(b, c[par[v]][v]);
        for (int v = t; v != s; v = par[v]) { c[par[v]][v] -= b; c[v][par[v]] += b; }
        f += b;
    }
    std::vector<int> vis(n, 0); std::queue<int> q; q.push(s); vis[s] = 1;
    while (!q.empty()) { int u = q.front(); q.pop(); for (int v = 0; v < n; ++v) if (!vis[v] && c[u][v] > 0) { vis[v] = 1; q.push(v); } }
    side = vis; return f;
}

static int fails = 0;
#define CHECK(c, ...) do { if (!(c)) { fails++; printf("FALHA: " __VA_ARGS__); printf("\n"); } } while (0)

static void testMaxflow() {
    std::mt19937 rng(1);
    for (int trial = 0; trial < 300; ++trial) {
        int n = 2 + rng() % 12;
        MaxFlow g; g.addNodes(n);
        std::vector<std::vector<long long>> c(n + 2, std::vector<long long>(n + 2, 0));
        int S = n, T = n + 1;
        for (int i = 0; i < n; ++i) {
            int a = rng() % 20, b = rng() % 20;
            if (rng() % 3 == 0) a = 0; if (rng() % 3 == 0) b = 0;
            g.addTWeights(i, a, b); c[S][i] += a; c[i][T] += b;
        }
        int m = rng() % (n * 3);
        for (int k = 0; k < m; ++k) {
            int i = rng() % n, j = rng() % n; if (i == j) continue;
            int a = rng() % 15, b = rng() % 15;
            g.addEdge(i, j, a, b); c[i][j] += a; c[j][i] += b;
        }
        long long f = g.solve();
        std::vector<int> side;
        long long fr = ek(n + 2, c, S, T, side);
        CHECK(f == fr, "maxflow trial %d: %lld != %lld", trial, (long long)f, fr);
        // o corte devolvido tem custo igual ao fluxo
        long long cutCost = 0;
        for (int u = 0; u < n + 2; ++u) for (int v = 0; v < n + 2; ++v) {
            bool su = (u == S) || (u < n && g.isSource(u)), sv = (v == S) || (v < n && g.isSource(v));
            if (u == T) su = false; if (v == T) sv = false;
            if (su && !sv) cutCost += c[u][v];
        }
        CHECK(cutCost == fr, "corte trial %d custo %lld fluxo %lld", trial, cutCost, fr);
    }
}

// imagem: fundo texturizado verde-azulado; objeto = disco laranja com borda suave + sombra
static Image8 makeImage(int W, int H, float cx, float cy, float R) {
    Image8 im; im.w = W; im.h = H; im.px.resize((size_t)W * H * 4);
    std::mt19937 rng(7); std::normal_distribution<float> nz(0, 10);
    for (int y = 0; y < H; ++y) for (int x = 0; x < W; ++x) {
        float bgR = 40 + 30 * std::sin(x * 0.05f), bgG = 120 + 25 * std::cos(y * 0.04f), bgB = 110;
        float d = std::hypot(x + .5f - cx, y + .5f - cy);
        float a = std::clamp(R - d + 0.5f, 0.f, 1.f);  // borda antisserrilhada
        float fr = 230, fg = 140 + 20 * std::sin((x + y) * 0.1f), fb = 40;
        uint8_t* p = &im.px[((size_t)y * W + x) * 4];
        p[0] = (uint8_t)std::clamp(bgR * (1 - a) + fr * a + nz(rng), 0.f, 255.f);
        p[1] = (uint8_t)std::clamp(bgG * (1 - a) + fg * a + nz(rng), 0.f, 255.f);
        p[2] = (uint8_t)std::clamp(bgB * (1 - a) + fb * a + nz(rng), 0.f, 255.f);
        p[3] = 255;
    }
    return im;
}

static double iou(const Mask8& m, float cx, float cy, float R, double& edgeErr) {
    double inter = 0, uni = 0, ee = 0; int en = 0;
    for (int y = 0; y < m.h; ++y) for (int x = 0; x < m.w; ++x) {
        float d = std::hypot(x + .5f - cx, y + .5f - cy);
        float gt = std::clamp(R - d + 0.5f, 0.f, 1.f);
        float v = m.v[(size_t)y * m.w + x] / 255.f;
        inter += std::min(gt, v); uni += std::max(gt, v);
        if (std::fabs(d - R) < 3) { ee += std::fabs(gt - v); en++; }
    }
    edgeErr = en ? ee / en : 0;
    return inter / uni;
}

static void writePgm(const char* f, const Mask8& m) {
    FILE* fp = fopen(f, "wb"); fprintf(fp, "P5 %d %d 255\n", m.w, m.h); fwrite(m.v.data(), 1, m.v.size(), fp); fclose(fp);
}

int main() {
    auto t0 = std::chrono::steady_clock::now();
    testMaxflow();
    printf("maxflow: 300 grafos aleatórios conferidos (%s)\n", fails ? "com falhas" : "ok");

    const int W = 2400, H = 1800; const float cx = 1300, cy = 900, R = 520;
    Image8 im = makeImage(W, H, cx, cy, R);
    Segmenter seg;
    auto tp = std::chrono::steady_clock::now();
    seg.prepare(im);
    double prepMs = std::chrono::duration<double, std::milli>(std::chrono::steady_clock::now() - tp).count();
    printf("prepare %dx%d -> %dx%d em %.0f ms\n", W, H, seg.ww, seg.wh, prepMs);

    // seleção rápida: um traço curto dentro do disco
    Mask8 sel;
    QuickSelectParams qp; qp.brushRadius = 40;
    for (int i = 0; i < 10; ++i) qp.stroke.push_back({cx - 200 + i * 40.f, cy + 30.f * std::sin(i * 0.7f)});
    auto tq = std::chrono::steady_clock::now();
    seg.quickSelect(sel, im, qp);
    double qMs = std::chrono::duration<double, std::milli>(std::chrono::steady_clock::now() - tq).count();
    double ee; double j = iou(sel, cx, cy, R, ee);
    printf("seleção rápida: IoU %.4f, erro médio na borda %.3f, %.0f ms\n", j, ee, qMs);
    CHECK(j > 0.97, "seleção rápida IoU baixo %.3f", j);
    CHECK(ee < 0.25, "seleção rápida borda ruim %.3f", ee);
    writePgm("quick.pgm", sel);

    // subtrair: traço fora do disco não deve mudar nada; traço na metade direita remove algo
    Mask8 sel2 = sel;
    QuickSelectParams sp; sp.brushRadius = 30; sp.subtract = true;
    sp.stroke = {{cx + 300, cy - 50}, {cx + 400, cy + 50}};
    seg.quickSelect(sel2, im, sp);
    double rem = 0; for (size_t i = 0; i < sel.v.size(); ++i) rem += (sel.v[i] - sel2.v[i]) / 255.0;
    printf("subtrair dentro do objeto de cor uniforme: %.0f px removidos\n", rem);

    // seleção de objeto: retângulo folgado ao redor do disco
    Mask8 obj;
    std::vector<Pt> rect = {{cx - R - 150, cy - R - 120}, {cx + R + 150, cy - R - 120}, {cx + R + 150, cy + R + 120}, {cx - R - 150, cy + R + 120}};
    auto to = std::chrono::steady_clock::now();
    seg.objectSelect(obj, im, rect, true);
    double oMs = std::chrono::duration<double, std::milli>(std::chrono::steady_clock::now() - to).count();
    j = iou(obj, cx, cy, R, ee);
    printf("seleção de objeto: IoU %.4f, erro médio na borda %.3f, %.0f ms\n", j, ee, oMs);
    CHECK(j > 0.97, "seleção de objeto IoU baixo %.3f", j);
    writePgm("object.pgm", obj);

    // varinha
    Mask8 wand;
    magicWand(wand, im, (int)cx, (int)cy, 60, true, true);
    j = iou(wand, cx, cy, R, ee);
    printf("varinha (tol 60): IoU %.4f\n", j);

    // refinar borda: seleção grosseira (polígono de 12 lados maior) → deve encostar na borda real
    Mask8 rough; std::vector<Pt> poly;
    for (int i = 0; i < 12; ++i) { float t = i / 12.f * 6.2831853f; poly.push_back({cx + (R + 25) * std::cos(t), cy + (R + 25) * std::sin(t)}); }
    rasterPolygon(rough, W, H, poly, true);
    double before = iou(rough, cx, cy, R, ee);
    RefineParams rp; rp.radius = 30; rp.contrast = 0.4f;
    refineEdge(rough, im, rp);
    double after = iou(rough, cx, cy, R, ee);
    printf("refinar borda: IoU %.4f -> %.4f (borda %.3f)\n", before, after, ee);
    CHECK(after > before, "refinar borda não melhorou");

    // RLE ida e volta
    auto enc = rleEncode(obj); Mask8 dec; rleDecode(enc, dec, W, H);
    CHECK(dec.v == obj.v, "RLE não confere");
    printf("RLE: %zu bytes -> %zu bytes\n", obj.v.size(), enc.size());

    printf("tempo total %.1f s · %s\n", std::chrono::duration<double>(std::chrono::steady_clock::now() - t0).count(), fails ? "FALHOU" : "TUDO OK");
    return fails ? 1 : 0;
}
