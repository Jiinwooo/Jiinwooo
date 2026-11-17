// SPDX-License-Identifier: MIT OR Apache-2.0
#include "selection.h"
#include "maxflow.h"
#include <algorithm>
#include <cmath>
#include <cstring>
#include <deque>

namespace bp {

// ------------------------------------------------------------------ utilidades de máscara

void combineMask(Mask8& cur, const Mask8& add, SelOp op) {
    if (op == SelOp::Replace || cur.empty()) {
        if (op == SelOp::Subtract) { if (cur.empty()) cur.reset(add.w, add.h, 0); }
        else if (op == SelOp::Intersect && cur.empty()) { cur.reset(add.w, add.h, 0); return; }
        else { cur = add; return; }
    }
    size_t n = cur.v.size();
    uint8_t* c = cur.v.data();
    const uint8_t* a = add.v.data();
    switch (op) {
        case SelOp::Add:       for (size_t i = 0; i < n; ++i) c[i] = std::max(c[i], a[i]); break;
        case SelOp::Subtract:  for (size_t i = 0; i < n; ++i) c[i] = std::min<int>(c[i], 255 - a[i]); break;
        case SelOp::Intersect: for (size_t i = 0; i < n; ++i) c[i] = std::min(c[i], a[i]); break;
        default: break;
    }
}

bool maskIsEmpty(const Mask8& m) {
    for (uint8_t v : m.v) if (v) return false;
    return true;
}

bool maskBounds(const Mask8& m, int& x0, int& y0, int& x1, int& y1) {
    x0 = m.w; y0 = m.h; x1 = 0; y1 = 0;
    for (int y = 0; y < m.h; ++y) {
        const uint8_t* r = &m.v[(size_t)y * m.w];
        int a = -1, b = -1;
        for (int x = 0; x < m.w; ++x) if (r[x]) { a = x; break; }
        if (a < 0) continue;
        for (int x = m.w - 1; x >= a; --x) if (r[x]) { b = x; break; }
        x0 = std::min(x0, a); x1 = std::max(x1, b + 1);
        y0 = std::min(y0, y); y1 = y + 1;
    }
    return x1 > x0 && y1 > y0;
}

// ------------------------------------------------------------------ polígonos

void ellipsePolygon(std::vector<Pt>& out, float x0, float y0, float x1, float y1) {
    out.clear();
    float cx = (x0 + x1) * .5f, cy = (y0 + y1) * .5f, rx = std::fabs(x1 - x0) * .5f, ry = std::fabs(y1 - y0) * .5f;
    int n = std::clamp((int)((rx + ry) * 0.6f), 32, 720);
    for (int i = 0; i < n; ++i) {
        float t = (float)i / n * 6.2831853f;
        out.push_back({cx + rx * std::cos(t), cy + ry * std::sin(t)});
    }
}

void rasterPolygon(Mask8& out, int w, int h, const std::vector<Pt>& pts, bool antialias) {
    out.reset(w, h, 0);
    if (pts.size() < 3) return;
    float minY = 1e30f, maxY = -1e30f;
    for (auto& p : pts) { minY = std::min(minY, p.y); maxY = std::max(maxY, p.y); }
    int y0 = std::max(0, (int)std::floor(minY)), y1 = std::min(h, (int)std::ceil(maxY) + 1);
    const int S = antialias ? 5 : 1;
    std::vector<float> acc(w);
    std::vector<float> xs;
    size_t n = pts.size();
    for (int y = y0; y < y1; ++y) {
        std::fill(acc.begin(), acc.end(), 0.f);
        bool any = false;
        for (int s = 0; s < S; ++s) {
            float sy = y + (s + 0.5f) / S;
            xs.clear();
            for (size_t i = 0; i < n; ++i) {
                const Pt& a = pts[i];
                const Pt& b = pts[(i + 1) % n];
                if ((a.y <= sy && b.y > sy) || (b.y <= sy && a.y > sy)) {
                    xs.push_back(a.x + (sy - a.y) / (b.y - a.y) * (b.x - a.x));
                }
            }
            std::sort(xs.begin(), xs.end());
            for (size_t k = 0; k + 1 < xs.size(); k += 2) {
                float xa = std::clamp(xs[k], 0.f, (float)w), xb = std::clamp(xs[k + 1], 0.f, (float)w);
                if (xb <= xa) continue;
                any = true;
                if (!antialias) {
                    int ia = (int)std::ceil(xa - 0.5f), ib = (int)std::ceil(xb - 0.5f);
                    for (int i = ia; i < ib && i < w; ++i) acc[i] += 1.f;
                    continue;
                }
                float wgt = 1.f / S;
                int ia = (int)xa, ib = (int)xb;
                if (ia == ib) { if (ia < w) acc[ia] += (xb - xa) * wgt; }
                else {
                    acc[ia] += (ia + 1 - xa) * wgt;
                    for (int i = ia + 1; i < ib; ++i) acc[i] += wgt;
                    if (ib < w) acc[ib] += (xb - ib) * wgt;
                }
            }
        }
        if (!any) continue;
        uint8_t* row = &out.v[(size_t)y * w];
        for (int x = 0; x < w; ++x) row[x] = (uint8_t)std::lround(std::clamp(acc[x], 0.f, 1.f) * 255.f);
    }
}

// ------------------------------------------------------------------ desfoque em caixa

static void boxBlurH(const float* src, float* dst, int w, int h, int r) {
    if (r <= 0) { std::memcpy(dst, src, sizeof(float) * w * h); return; }
    float inv = 1.f / (2 * r + 1);
    for (int y = 0; y < h; ++y) {
        const float* s = src + (size_t)y * w;
        float* d = dst + (size_t)y * w;
        float acc = s[0] * (r + 1);
        for (int i = 1; i <= r; ++i) acc += s[std::min(i, w - 1)];
        for (int x = 0; x < w; ++x) {
            d[x] = acc * inv;
            acc += s[std::min(x + r + 1, w - 1)] - s[std::max(x - r, 0)];
        }
    }
}

static void boxBlurV(const float* src, float* dst, int w, int h, int r) {
    if (r <= 0) { std::memcpy(dst, src, sizeof(float) * w * h); return; }
    float inv = 1.f / (2 * r + 1);
    std::vector<float> acc(w);
    for (int x = 0; x < w; ++x) {
        acc[x] = src[x] * (r + 1);
        for (int i = 1; i <= r; ++i) acc[x] += src[(size_t)std::min(i, h - 1) * w + x];
    }
    for (int y = 0; y < h; ++y) {
        const float* add = src + (size_t)std::min(y + r + 1, h - 1) * w;
        const float* sub = src + (size_t)std::max(y - r, 0) * w;
        float* d = dst + (size_t)y * w;
        for (int x = 0; x < w; ++x) { d[x] = acc[x] * inv; acc[x] += add[x] - sub[x]; }
    }
}

static void boxBlur(std::vector<float>& v, int w, int h, int r) {
    if (r <= 0) return;
    std::vector<float> t(v.size());
    boxBlurH(v.data(), t.data(), w, h, r);
    boxBlurV(t.data(), v.data(), w, h, r);
}

void featherMask(Mask8& m, float radius) {
    if (radius < 0.5f || m.empty()) return;
    // três caixas ≈ gaussiana com sigma = radius / 2
    float sigma = radius * 0.5f;
    int r = std::max(1, (int)std::lround(std::sqrt(12.f * sigma * sigma / 3.f + 1.f) * 0.5f));
    std::vector<float> f(m.v.begin(), m.v.end());
    for (int k = 0; k < 3; ++k) boxBlur(f, m.w, m.h, r);
    for (size_t i = 0; i < f.size(); ++i) m.v[i] = (uint8_t)std::lround(std::clamp(f[i], 0.f, 255.f));
}

// ------------------------------------------------------------------ varinha mágica

static inline void unpremul(const uint8_t* p, int& r, int& g, int& b) {
    int a = p[3];
    if (a == 0) { r = g = b = 0; return; }
    if (a == 255) { r = p[0]; g = p[1]; b = p[2]; return; }
    r = std::min(255, p[0] * 255 / a); g = std::min(255, p[1] * 255 / a); b = std::min(255, p[2] * 255 / a);
}

static void antialiasEdges(Mask8& m) {
    Mask8 src = m;
    int w = m.w, h = m.h;
    for (int y = 0; y < h; ++y) for (int x = 0; x < w; ++x) {
        uint8_t c = src.v[(size_t)y * w + x];
        bool edge = false;
        int sum = 0, n = 0;
        for (int dy = -1; dy <= 1; ++dy) for (int dx = -1; dx <= 1; ++dx) {
            int xx = std::clamp(x + dx, 0, w - 1), yy = std::clamp(y + dy, 0, h - 1);
            uint8_t v = src.v[(size_t)yy * w + xx];
            if (v != c) edge = true;
            sum += v; n++;
        }
        if (edge) m.v[(size_t)y * w + x] = (uint8_t)((sum + n / 2) / n);
    }
}

void magicWand(Mask8& out, const Image8& img, int sx, int sy, int tol, bool contiguous, bool antialias) {
    int w = img.w, h = img.h;
    out.reset(w, h, 0);
    if (sx < 0 || sy < 0 || sx >= w || sy >= h) return;
    int r0, g0, b0;
    const uint8_t* s = &img.px[((size_t)sy * w + sx) * 4];
    unpremul(s, r0, g0, b0);
    int a0 = s[3];
    auto match = [&](int x, int y) {
        const uint8_t* p = &img.px[((size_t)y * w + x) * 4];
        int r, g, b; unpremul(p, r, g, b);
        return std::abs(r - r0) <= tol && std::abs(g - g0) <= tol && std::abs(b - b0) <= tol && std::abs(p[3] - a0) <= tol;
    };
    if (!contiguous) {
        for (int y = 0; y < h; ++y) for (int x = 0; x < w; ++x) if (match(x, y)) out.v[(size_t)y * w + x] = 255;
    } else {
        // preenchimento por varredura de linhas
        std::vector<std::pair<int, int>> stack;
        stack.push_back({sx, sy});
        while (!stack.empty()) {
            auto [x, y] = stack.back(); stack.pop_back();
            uint8_t* row = &out.v[(size_t)y * w];
            if (row[x]) continue;
            int l = x, r = x;
            while (l > 0 && !row[l - 1] && match(l - 1, y)) --l;
            while (r < w - 1 && !row[r + 1] && match(r + 1, y)) ++r;
            for (int i = l; i <= r; ++i) row[i] = 255;
            for (int ny : {y - 1, y + 1}) {
                if (ny < 0 || ny >= h) continue;
                uint8_t* nrow = &out.v[(size_t)ny * w];
                bool inRun = false;
                for (int i = l; i <= r; ++i) {
                    bool ok = !nrow[i] && match(i, ny);
                    if (ok && !inRun) { stack.push_back({i, ny}); inRun = true; }
                    else if (!ok) inRun = false;
                }
            }
        }
    }
    if (antialias) antialiasEdges(out);
}

// ------------------------------------------------------------------ GMM (mistura de gaussianas)

namespace {

struct Gmm {
    static constexpr int K = 5;
    double pi[K], mu[K][3], inv[K][9], logNorm[K];
    int used = 0;

    static bool invert3(const double* c, double* inv, double& det) {
        det = c[0] * (c[4] * c[8] - c[5] * c[7]) - c[1] * (c[3] * c[8] - c[5] * c[6]) + c[2] * (c[3] * c[7] - c[4] * c[6]);
        if (det <= 1e-12) return false;
        double id = 1.0 / det;
        inv[0] = (c[4] * c[8] - c[5] * c[7]) * id;
        inv[1] = (c[2] * c[7] - c[1] * c[8]) * id;
        inv[2] = (c[1] * c[5] - c[2] * c[4]) * id;
        inv[3] = (c[5] * c[6] - c[3] * c[8]) * id;
        inv[4] = (c[0] * c[8] - c[2] * c[6]) * id;
        inv[5] = (c[2] * c[3] - c[0] * c[5]) * id;
        inv[6] = (c[3] * c[7] - c[4] * c[6]) * id;
        inv[7] = (c[1] * c[6] - c[0] * c[7]) * id;
        inv[8] = (c[0] * c[4] - c[1] * c[3]) * id;
        return true;
    }

    // amostras: rgb 0..1 intercaladas
    void fit(const std::vector<float>& s) {
        int n = (int)s.size() / 3;
        used = 0;
        if (n == 0) return;
        int k = std::min(K, std::max(1, n / 8));
        // k-means++ determinístico
        std::vector<int> assign(n, 0);
        double c[K][3];
        uint32_t rng = 0x9E3779B9u;
        auto rnd = [&]() { rng ^= rng << 13; rng ^= rng >> 17; rng ^= rng << 5; return rng; };
        int first = rnd() % n;
        for (int d = 0; d < 3; ++d) c[0][d] = s[first * 3 + d];
        std::vector<double> dist(n, 1e30);
        for (int ci = 1; ci < k; ++ci) {
            double total = 0;
            for (int i = 0; i < n; ++i) {
                double dd = 0;
                for (int d = 0; d < 3; ++d) { double t = s[i * 3 + d] - c[ci - 1][d]; dd += t * t; }
                dist[i] = std::min(dist[i], dd);
                total += dist[i];
            }
            double r = (rnd() / 4294967296.0) * total, acc = 0;
            int pick = n - 1;
            for (int i = 0; i < n; ++i) { acc += dist[i]; if (acc >= r) { pick = i; break; } }
            for (int d = 0; d < 3; ++d) c[ci][d] = s[pick * 3 + d];
        }
        for (int it = 0; it < 8; ++it) {
            double sum[K][3] = {}; int cnt[K] = {};
            for (int i = 0; i < n; ++i) {
                int best = 0; double bd = 1e30;
                for (int ci = 0; ci < k; ++ci) {
                    double dd = 0;
                    for (int d = 0; d < 3; ++d) { double t = s[i * 3 + d] - c[ci][d]; dd += t * t; }
                    if (dd < bd) { bd = dd; best = ci; }
                }
                assign[i] = best; cnt[best]++;
                for (int d = 0; d < 3; ++d) sum[best][d] += s[i * 3 + d];
            }
            for (int ci = 0; ci < k; ++ci) if (cnt[ci]) for (int d = 0; d < 3; ++d) c[ci][d] = sum[ci][d] / cnt[ci];
        }
        // estatísticas por componente
        for (int ci = 0; ci < k; ++ci) {
            double m[3] = {}, cov[9] = {}; int cnt = 0;
            for (int i = 0; i < n; ++i) if (assign[i] == ci) { cnt++; for (int d = 0; d < 3; ++d) m[d] += s[i * 3 + d]; }
            if (cnt < 2) continue;
            for (double& v : m) v /= cnt;
            for (int i = 0; i < n; ++i) if (assign[i] == ci) {
                double t[3] = {s[i * 3] - m[0], s[i * 3 + 1] - m[1], s[i * 3 + 2] - m[2]};
                for (int a = 0; a < 3; ++a) for (int b = 0; b < 3; ++b) cov[a * 3 + b] += t[a] * t[b];
            }
            for (double& v : cov) v /= cnt;
            for (int d = 0; d < 3; ++d) cov[d * 4] += 1e-4;   // regularização (~ 2,5 níveis de 255)
            double det;
            int u = used;
            if (!invert3(cov, inv[u], det)) continue;
            pi[u] = (double)cnt / n;
            for (int d = 0; d < 3; ++d) mu[u][d] = m[d];
            logNorm[u] = -0.5 * std::log(det) - 1.5 * std::log(2 * M_PI);
            used++;
        }
    }

    // -log p(c)
    float cost(const float* c) const {
        if (!used) return 10.f;
        double best = -1e300, terms[K];
        for (int k = 0; k < used; ++k) {
            double t[3] = {c[0] - mu[k][0], c[1] - mu[k][1], c[2] - mu[k][2]};
            const double* iv = inv[k];
            double q = t[0] * (iv[0] * t[0] + iv[1] * t[1] + iv[2] * t[2]) +
                       t[1] * (iv[3] * t[0] + iv[4] * t[1] + iv[5] * t[2]) +
                       t[2] * (iv[6] * t[0] + iv[7] * t[1] + iv[8] * t[2]);
            terms[k] = std::log(pi[k]) + logNorm[k] - 0.5 * q;
            best = std::max(best, terms[k]);
        }
        double sum = 0;
        for (int k = 0; k < used; ++k) sum += std::exp(terms[k] - best);
        double lp = best + std::log(sum);
        // densidade em espaço 0..1: desloca para que valores típicos fiquem em ~0..30
        return (float)std::clamp(-lp + 8.0, 0.0, 40.0);
    }
};

void sampleFrom(const std::vector<float>& rgb, const std::vector<uint8_t>& sel, uint8_t want,
                std::vector<float>& out, int maxN) {
    out.clear();
    size_t n = sel.size(), cnt = 0;
    for (size_t i = 0; i < n; ++i) if (sel[i] == want) cnt++;
    if (!cnt) return;
    size_t step = std::max<size_t>(1, cnt / maxN), k = 0;
    for (size_t i = 0; i < n; ++i) if (sel[i] == want) {
        if (k++ % step) continue;
        out.insert(out.end(), {rgb[i * 3], rgb[i * 3 + 1], rgb[i * 3 + 2]});
    }
}

// distância (chanfro 3-4) até os pixels marcados
void chamfer(const std::vector<uint8_t>& seed, int w, int h, std::vector<float>& d) {
    const float INF = 1e9f;
    d.assign(seed.size(), INF);
    for (size_t i = 0; i < seed.size(); ++i) if (seed[i]) d[i] = 0;
    const float a = 1.f, b = 1.4142f;
    for (int y = 0; y < h; ++y) for (int x = 0; x < w; ++x) {
        float& v = d[(size_t)y * w + x];
        if (x > 0) v = std::min(v, d[(size_t)y * w + x - 1] + a);
        if (y > 0) {
            v = std::min(v, d[(size_t)(y - 1) * w + x] + a);
            if (x > 0) v = std::min(v, d[(size_t)(y - 1) * w + x - 1] + b);
            if (x < w - 1) v = std::min(v, d[(size_t)(y - 1) * w + x + 1] + b);
        }
    }
    for (int y = h - 1; y >= 0; --y) for (int x = w - 1; x >= 0; --x) {
        float& v = d[(size_t)y * w + x];
        if (x < w - 1) v = std::min(v, d[(size_t)y * w + x + 1] + a);
        if (y < h - 1) {
            v = std::min(v, d[(size_t)(y + 1) * w + x] + a);
            if (x < w - 1) v = std::min(v, d[(size_t)(y + 1) * w + x + 1] + b);
            if (x > 0) v = std::min(v, d[(size_t)(y + 1) * w + x - 1] + b);
        }
    }
}

// mantém só os componentes (vizinhança 4) de label==1 que tocam em keep
void keepConnected(std::vector<uint8_t>& label, const std::vector<uint8_t>& keep, int w, int h) {
    std::vector<uint8_t> vis(label.size(), 0);
    std::vector<int> st;
    for (size_t i = 0; i < label.size(); ++i) if (keep[i] && label[i] && !vis[i]) { vis[i] = 1; st.push_back((int)i); }
    while (!st.empty()) {
        int i = st.back(); st.pop_back();
        int x = i % w, y = i / w;
        int nb[4] = {x > 0 ? i - 1 : -1, x < w - 1 ? i + 1 : -1, y > 0 ? i - w : -1, y < h - 1 ? i + w : -1};
        for (int j : nb) if (j >= 0 && label[j] && !vis[j]) { vis[j] = 1; st.push_back(j); }
    }
    for (size_t i = 0; i < label.size(); ++i) if (label[i] && !vis[i]) label[i] = 0;
}

// remove componentes menores que frac × o maior
void keepLargest(std::vector<uint8_t>& label, int w, int h, float frac) {
    std::vector<int> comp(label.size(), -1), sizes;
    std::vector<int> st;
    for (size_t s = 0; s < label.size(); ++s) {
        if (!label[s] || comp[s] >= 0) continue;
        int id = (int)sizes.size(), cnt = 0;
        comp[s] = id; st.push_back((int)s);
        while (!st.empty()) {
            int i = st.back(); st.pop_back(); cnt++;
            int x = i % w, y = i / w;
            int nb[4] = {x > 0 ? i - 1 : -1, x < w - 1 ? i + 1 : -1, y > 0 ? i - w : -1, y < h - 1 ? i + w : -1};
            for (int j : nb) if (j >= 0 && label[j] && comp[j] < 0) { comp[j] = id; st.push_back(j); }
        }
        sizes.push_back(cnt);
    }
    if (sizes.empty()) return;
    int mx = *std::max_element(sizes.begin(), sizes.end());
    for (size_t i = 0; i < label.size(); ++i) if (label[i] && sizes[comp[i]] < mx * frac) label[i] = 0;
    // fecha buracos pequenos do fundo dentro do objeto
    std::vector<uint8_t> inv(label.size());
    for (size_t i = 0; i < label.size(); ++i) inv[i] = !label[i];
    std::fill(comp.begin(), comp.end(), -1); sizes.clear();
    std::vector<uint8_t> touches;
    for (size_t s = 0; s < inv.size(); ++s) {
        if (!inv[s] || comp[s] >= 0) continue;
        int id = (int)sizes.size(), cnt = 0; bool edge = false;
        comp[s] = id; st.push_back((int)s);
        while (!st.empty()) {
            int i = st.back(); st.pop_back(); cnt++;
            int x = i % w, y = i / w;
            if (x == 0 || y == 0 || x == w - 1 || y == h - 1) edge = true;
            int nb[4] = {x > 0 ? i - 1 : -1, x < w - 1 ? i + 1 : -1, y > 0 ? i - w : -1, y < h - 1 ? i + w : -1};
            for (int j : nb) if (j >= 0 && inv[j] && comp[j] < 0) { comp[j] = id; st.push_back(j); }
        }
        sizes.push_back(cnt); touches.push_back(edge);
    }
    int holeMax = std::max(16, (int)(mx * 0.002f));
    for (size_t i = 0; i < inv.size(); ++i) if (inv[i] && !touches[comp[i]] && sizes[comp[i]] < holeMax) label[i] = 1;
}

} // namespace

// ------------------------------------------------------------------ Segmenter

void Segmenter::prepare(const Image8& img, int workMax) {
    ready_ = false;
    if (img.empty()) return;
    scale = std::max(1.f, (float)std::max(img.w, img.h) / workMax);
    ww = std::max(1, (int)std::lround(img.w / scale));
    wh = std::max(1, (int)std::lround(img.h / scale));
    float sx = (float)img.w / ww, sy = (float)img.h / wh;
    scale = sx;
    rgb.assign((size_t)ww * wh * 3, 0.f);
    // média por área
    for (int y = 0; y < wh; ++y) {
        int ya = (int)(y * sy), yb = std::max(ya + 1, std::min(img.h, (int)((y + 1) * sy)));
        for (int x = 0; x < ww; ++x) {
            int xa = (int)(x * sx), xb = std::max(xa + 1, std::min(img.w, (int)((x + 1) * sx)));
            float acc[3] = {0, 0, 0}; int n = 0;
            for (int yy = ya; yy < yb; ++yy) for (int xx = xa; xx < xb; ++xx) {
                int r, g, b; unpremul(&img.px[((size_t)yy * img.w + xx) * 4], r, g, b);
                acc[0] += r; acc[1] += g; acc[2] += b; n++;
            }
            float* d = &rgb[((size_t)y * ww + x) * 3];
            for (int c = 0; c < 3; ++c) d[c] = acc[c] / (n * 255.f);
        }
    }
    buildEdges();
    ready_ = true;
}

void Segmenter::buildEdges() {
    size_t n = (size_t)ww * wh;
    auto d2 = [&](size_t a, size_t b) {
        float s = 0; for (int c = 0; c < 3; ++c) { float t = rgb[a * 3 + c] - rgb[b * 3 + c]; s += t * t; } return s;
    };
    double sum = 0; size_t cnt = 0;
    for (int y = 0; y < wh; ++y) for (int x = 0; x < ww; ++x) {
        size_t i = (size_t)y * ww + x;
        if (x < ww - 1) { sum += d2(i, i + 1); cnt++; }
        if (y < wh - 1) { sum += d2(i, i + ww); cnt++; }
    }
    beta_ = cnt && sum > 0 ? (float)(1.0 / (2.0 * sum / cnt)) : 0.f;
    const float gamma = 50.f, SC = 100.f, invS2 = 1.f / 1.41421356f;
    wR_.assign(n, 0); wD_.assign(n, 0); wDR_.assign(n, 0); wDL_.assign(n, 0);
    for (int y = 0; y < wh; ++y) for (int x = 0; x < ww; ++x) {
        size_t i = (size_t)y * ww + x;
        if (x < ww - 1) wR_[i] = (int32_t)(SC * gamma * std::exp(-beta_ * d2(i, i + 1)));
        if (y < wh - 1) {
            wD_[i] = (int32_t)(SC * gamma * std::exp(-beta_ * d2(i, i + ww)));
            if (x < ww - 1) wDR_[i] = (int32_t)(SC * gamma * invS2 * std::exp(-beta_ * d2(i, i + ww + 1)));
            if (x > 0) wDL_[i] = (int32_t)(SC * gamma * invS2 * std::exp(-beta_ * d2(i, i + ww - 1)));
        }
    }
}

// hard: 0 livre, 1 primeiro plano fixo, 2 fundo fixo. label de saída: 1 = primeiro plano.
void Segmenter::cut(const std::vector<float>& costFg, const std::vector<float>& costBg,
                    const std::vector<uint8_t>& hard, std::vector<uint8_t>& label) {
    size_t n = (size_t)ww * wh;
    MaxFlow g((int)n, (int)n * 4);
    g.addNodes((int)n);
    const float SC = 100.f;
    const int32_t INF = 1 << 28;
    for (size_t i = 0; i < n; ++i) {
        if (hard[i] == 1) g.addTWeights((int)i, INF, 0);
        else if (hard[i] == 2) g.addTWeights((int)i, 0, INF);
        else g.addTWeights((int)i, (int32_t)(costBg[i] * SC), (int32_t)(costFg[i] * SC));
    }
    for (int y = 0; y < wh; ++y) for (int x = 0; x < ww; ++x) {
        int i = y * ww + x;
        if (wR_[i]) g.addEdge(i, i + 1, wR_[i], wR_[i]);
        if (wD_[i]) g.addEdge(i, i + ww, wD_[i], wD_[i]);
        if (wDR_[i]) g.addEdge(i, i + ww + 1, wDR_[i], wDR_[i]);
        if (wDL_[i]) g.addEdge(i, i + ww - 1, wDL_[i], wDL_[i]);
    }
    g.solve();
    label.resize(n);
    for (size_t i = 0; i < n; ++i) label[i] = g.isSource((int)i) ? 1 : 0;
}

// máscara do documento → tamanho de trabalho (média por área, 0..1)
static void maskToWork(const Mask8& m, int ww, int wh, std::vector<float>& out) {
    out.assign((size_t)ww * wh, 0.f);
    if (m.empty()) return;
    float sx = (float)m.w / ww, sy = (float)m.h / wh;
    for (int y = 0; y < wh; ++y) {
        int ya = (int)(y * sy), yb = std::max(ya + 1, std::min(m.h, (int)((y + 1) * sy)));
        for (int x = 0; x < ww; ++x) {
            int xa = (int)(x * sx), xb = std::max(xa + 1, std::min(m.w, (int)((x + 1) * sx)));
            int acc = 0, n = 0;
            for (int yy = ya; yy < yb; ++yy) for (int xx = xa; xx < xb; ++xx) { acc += m.v[(size_t)yy * m.w + xx]; n++; }
            out[(size_t)y * ww + x] = acc / (255.f * n);
        }
    }
}

static void stampStroke(std::vector<uint8_t>& seed, int ww, int wh, const std::vector<Pt>& stroke, float scale, float radiusDoc) {
    seed.assign((size_t)ww * wh, 0);
    float r = std::max(1.0f, radiusDoc / scale);
    auto disc = [&](float cx, float cy) {
        int x0 = std::max(0, (int)(cx - r)), x1 = std::min(ww - 1, (int)(cx + r) + 1);
        int y0 = std::max(0, (int)(cy - r)), y1 = std::min(wh - 1, (int)(cy + r) + 1);
        for (int y = y0; y <= y1; ++y) for (int x = x0; x <= x1; ++x) {
            float dx = x + .5f - cx, dy = y + .5f - cy;
            if (dx * dx + dy * dy <= r * r) seed[(size_t)y * ww + x] = 1;
        }
    };
    for (size_t i = 0; i < stroke.size(); ++i) {
        float ax = stroke[i].x / scale, ay = stroke[i].y / scale;
        if (i == 0) { disc(ax, ay); continue; }
        float bx = stroke[i - 1].x / scale, by = stroke[i - 1].y / scale;
        float len = std::hypot(ax - bx, ay - by);
        int steps = std::max(1, (int)(len / std::max(0.5f, r * 0.4f)));
        for (int s = 1; s <= steps; ++s) { float t = (float)s / steps; disc(bx + (ax - bx) * t, by + (ay - by) * t); }
    }
}

void Segmenter::quickSelect(Mask8& sel, const Image8& img, const QuickSelectParams& p) {
    if (!ready_ || p.stroke.empty()) return;
    size_t n = (size_t)ww * wh;
    if (sel.empty()) sel.reset(img.w, img.h, 0);

    std::vector<uint8_t> seed;
    stampStroke(seed, ww, wh, p.stroke, scale, p.brushRadius);
    std::vector<float> cur;
    maskToWork(sel, ww, wh, cur);

    std::vector<float> dist;
    chamfer(seed, ww, wh, dist);
    float rW = std::max(1.f, p.brushRadius / scale);
    float R0 = std::max(rW * 4.f, 0.06f * std::max(ww, wh));

    // amostras dos modelos de cor
    std::vector<uint8_t> cls(n, 0);  // 1 = amostra de primeiro plano, 2 = amostra de fundo
    for (size_t i = 0; i < n; ++i) {
        if (!p.subtract) {
            if (seed[i]) cls[i] = 1;
            else if (cur[i] < 0.05f && dist[i] > rW * 1.5f) cls[i] = 2;
        } else {
            if (seed[i]) cls[i] = 2;
            else if (cur[i] > 0.95f && dist[i] > rW * 1.5f) cls[i] = 1;
        }
    }
    std::vector<float> sFg, sBg;
    sampleFrom(rgb, cls, 1, sFg, 20000);
    sampleFrom(rgb, cls, 2, sBg, 20000);
    Gmm fg, bg; fg.fit(sFg); bg.fit(sBg);

    std::vector<float> cFg(n), cBg(n);
    std::vector<uint8_t> hard(n, 0);
    const float lambdaD = 2.0f;
    for (size_t i = 0; i < n; ++i) {
        const float* c = &rgb[i * 3];
        cFg[i] = fg.cost(c);
        cBg[i] = bg.cost(c);
        float far = std::max(0.f, dist[i] - R0) / R0 * lambdaD;
        if (!p.subtract) {
            cFg[i] += far;                       // longe do traço: tende a não entrar
            if (seed[i] || cur[i] > 0.95f) hard[i] = 1;
        } else {
            cBg[i] += far;                       // longe do traço: tende a não sair
            if (seed[i]) hard[i] = 2;
            else if (cur[i] < 0.05f) hard[i] = 2;
        }
    }
    std::vector<uint8_t> label;
    cut(cFg, cBg, hard, label);

    // região alterada (no tamanho de trabalho)
    std::vector<uint8_t> changed(n, 0);
    if (!p.subtract) {
        keepConnected(label, seed, ww, wh);
        for (size_t i = 0; i < n; ++i) changed[i] = label[i] && cur[i] <= 0.95f;
    } else {
        std::vector<uint8_t> removed(n);
        for (size_t i = 0; i < n; ++i) removed[i] = !label[i] && cur[i] > 0.05f;
        keepConnected(removed, seed, ww, wh);
        for (size_t i = 0; i < n; ++i) { changed[i] = removed[i]; label[i] = removed[i]; }
    }
    int bx0 = ww, by0 = wh, bx1 = -1, by1 = -1;
    for (int y = 0; y < wh; ++y) for (int x = 0; x < ww; ++x) if (changed[(size_t)y * ww + x]) {
        bx0 = std::min(bx0, x); bx1 = std::max(bx1, x); by0 = std::min(by0, y); by1 = std::max(by1, y);
    }
    if (bx1 < 0) return;

    // resolução cheia: região alterada + margem
    int margin = 6;
    int rx0 = std::max(0, (int)((bx0 - margin) * scale)), ry0 = std::max(0, (int)((by0 - margin) * scale));
    int rx1 = std::min(img.w, (int)((bx1 + 1 + margin) * scale) + 1), ry1 = std::min(img.h, (int)((by1 + 1 + margin) * scale) + 1);
    int rw = rx1 - rx0, rh = ry1 - ry0;

    std::vector<float> pw(n);
    for (size_t i = 0; i < n; ++i) pw[i] = label[i] ? 1.f : 0.f;
    std::vector<float> q;
    if (p.refineEdges) guidedUpsample(*this, img, pw, 3, 2e-4f, rx0, ry0, rx1, ry1, q);
    else {
        q.assign((size_t)rw * rh, 0.f);
        for (int y = 0; y < rh; ++y) for (int x = 0; x < rw; ++x) {
            int sx = std::min(ww - 1, (int)((rx0 + x + .5f) / scale)), sy = std::min(wh - 1, (int)((ry0 + y + .5f) / scale));
            q[(size_t)y * rw + x] = pw[(size_t)sy * ww + sx];
        }
    }
    for (int y = 0; y < rh; ++y) {
        uint8_t* row = &sel.v[(size_t)(ry0 + y) * sel.w + rx0];
        for (int x = 0; x < rw; ++x) {
            float a = q[(size_t)y * rw + x];
            a = std::clamp((a - 0.5f) * 1.6f + 0.5f, 0.f, 1.f);   // borda firme, sem halo
            int v = (int)std::lround(a * 255.f);
            if (!p.subtract) row[x] = (uint8_t)std::max<int>(row[x], v);
            else row[x] = (uint8_t)std::min<int>(row[x], 255 - v);
        }
    }
}

void Segmenter::objectSelect(Mask8& out, const Image8& img, const std::vector<Pt>& region, bool refineEdges) {
    out.reset(img.w, img.h, 0);
    if (!ready_ || region.size() < 3) return;
    size_t n = (size_t)ww * wh;
    std::vector<Pt> pw;
    for (auto& p : region) pw.push_back({p.x / scale, p.y / scale});
    Mask8 inside;
    rasterPolygon(inside, ww, wh, pw, false);
    std::vector<uint8_t> hard(n, 0), label(n, 0);
    size_t inCount = 0;
    for (size_t i = 0; i < n; ++i) {
        if (inside.v[i] < 128) hard[i] = 2; else { label[i] = 1; inCount++; }
    }
    if (inCount < 16) return;
    // borda da imagem conta como fundo quando o retângulo encosta nela? Não: o objeto pode tocar.
    std::vector<float> cFg(n), cBg(n), sFg, sBg;
    for (int it = 0; it < 5; ++it) {
        std::vector<uint8_t> cls(n, 0);
        for (size_t i = 0; i < n; ++i) cls[i] = label[i] ? 1 : 2;
        sampleFrom(rgb, cls, 1, sFg, 25000);
        sampleFrom(rgb, cls, 2, sBg, 25000);
        Gmm fg, bg; fg.fit(sFg); bg.fit(sBg);
        for (size_t i = 0; i < n; ++i) { cFg[i] = fg.cost(&rgb[i * 3]); cBg[i] = bg.cost(&rgb[i * 3]); }
        std::vector<uint8_t> nl;
        cut(cFg, cBg, hard, nl);
        bool same = nl == label;
        label.swap(nl);
        if (same) break;
    }
    keepLargest(label, ww, wh, 0.12f);

    std::vector<float> p(n);
    for (size_t i = 0; i < n; ++i) p[i] = label[i] ? 1.f : 0.f;
    std::vector<float> q;
    if (refineEdges) guidedUpsample(*this, img, p, 3, 2e-4f, 0, 0, img.w, img.h, q);
    else {
        q.assign((size_t)img.w * img.h, 0.f);
        for (int y = 0; y < img.h; ++y) for (int x = 0; x < img.w; ++x) {
            int sx = std::min(ww - 1, (int)((x + .5f) / scale)), sy = std::min(wh - 1, (int)((y + .5f) / scale));
            q[(size_t)y * img.w + x] = p[(size_t)sy * ww + sx];
        }
    }
    for (size_t i = 0; i < q.size(); ++i) {
        float a = std::clamp((q[i] - 0.5f) * 1.6f + 0.5f, 0.f, 1.f);
        out.v[i] = (uint8_t)std::lround(a * 255.f);
    }
}

// ------------------------------------------------------------------ filtro guiado rápido

void guidedUpsample(const Segmenter& seg, const Image8& img, const std::vector<float>& p,
                    int r, float eps, int rx0, int ry0, int rx1, int ry1, std::vector<float>& out) {
    int w = seg.ww, h = seg.wh;
    size_t n = (size_t)w * h;
    const std::vector<float>& I = seg.rgb;
    // médias em caixa no tamanho de trabalho
    std::vector<float> mI[3], mP(p), mIp[3], vII[6];
    for (int c = 0; c < 3; ++c) {
        mI[c].resize(n); mIp[c].resize(n);
        for (size_t i = 0; i < n; ++i) { mI[c][i] = I[i * 3 + c]; mIp[c][i] = I[i * 3 + c] * p[i]; }
        boxBlur(mI[c], w, h, r); boxBlur(mIp[c], w, h, r);
    }
    boxBlur(mP, w, h, r);
    const int pairs[6][2] = {{0, 0}, {0, 1}, {0, 2}, {1, 1}, {1, 2}, {2, 2}};
    for (int k = 0; k < 6; ++k) {
        vII[k].resize(n);
        int a = pairs[k][0], b = pairs[k][1];
        for (size_t i = 0; i < n; ++i) vII[k][i] = I[i * 3 + a] * I[i * 3 + b];
        boxBlur(vII[k], w, h, r);
    }
    std::vector<float> A[3], B(n);
    for (int c = 0; c < 3; ++c) A[c].resize(n);
    for (size_t i = 0; i < n; ++i) {
        double S[9];
        double crr = vII[0][i] - mI[0][i] * mI[0][i] + eps, crg = vII[1][i] - mI[0][i] * mI[1][i],
               crb = vII[2][i] - mI[0][i] * mI[2][i], cgg = vII[3][i] - mI[1][i] * mI[1][i] + eps,
               cgb = vII[4][i] - mI[1][i] * mI[2][i], cbb = vII[5][i] - mI[2][i] * mI[2][i] + eps;
        S[0] = crr; S[1] = crg; S[2] = crb; S[3] = crg; S[4] = cgg; S[5] = cgb; S[6] = crb; S[7] = cgb; S[8] = cbb;
        double cov[3] = {mIp[0][i] - mI[0][i] * mP[i], mIp[1][i] - mI[1][i] * mP[i], mIp[2][i] - mI[2][i] * mP[i]};
        double det = S[0] * (S[4] * S[8] - S[5] * S[7]) - S[1] * (S[3] * S[8] - S[5] * S[6]) + S[2] * (S[3] * S[7] - S[4] * S[6]);
        double a[3] = {0, 0, 0};
        if (std::fabs(det) > 1e-18) {
            double id = 1.0 / det;
            double inv[9] = {
                (S[4] * S[8] - S[5] * S[7]) * id, (S[2] * S[7] - S[1] * S[8]) * id, (S[1] * S[5] - S[2] * S[4]) * id,
                (S[5] * S[6] - S[3] * S[8]) * id, (S[0] * S[8] - S[2] * S[6]) * id, (S[2] * S[3] - S[0] * S[5]) * id,
                (S[3] * S[7] - S[4] * S[6]) * id, (S[1] * S[6] - S[0] * S[7]) * id, (S[0] * S[4] - S[1] * S[3]) * id};
            for (int c = 0; c < 3; ++c) a[c] = inv[c * 3] * cov[0] + inv[c * 3 + 1] * cov[1] + inv[c * 3 + 2] * cov[2];
        }
        for (int c = 0; c < 3; ++c) A[c][i] = (float)a[c];
        B[i] = (float)(mP[i] - a[0] * mI[0][i] - a[1] * mI[1][i] - a[2] * mI[2][i]);
    }
    for (int c = 0; c < 3; ++c) boxBlur(A[c], w, h, r);
    boxBlur(B, w, h, r);

    int rw = rx1 - rx0, rh = ry1 - ry0;
    out.assign((size_t)rw * rh, 0.f);
    float sx = (float)w / img.w, sy = (float)h / img.h;
    for (int y = 0; y < rh; ++y) {
        int Y = ry0 + y;
        float fy = (Y + 0.5f) * sy - 0.5f;
        int y0 = std::clamp((int)std::floor(fy), 0, h - 1), y1 = std::min(y0 + 1, h - 1);
        float ty = std::clamp(fy - y0, 0.f, 1.f);
        for (int x = 0; x < rw; ++x) {
            int X = rx0 + x;
            float fx = (X + 0.5f) * sx - 0.5f;
            int x0 = std::clamp((int)std::floor(fx), 0, w - 1), x1 = std::min(x0 + 1, w - 1);
            float tx = std::clamp(fx - x0, 0.f, 1.f);
            auto bil = [&](const std::vector<float>& v) {
                float a = v[(size_t)y0 * w + x0] * (1 - tx) + v[(size_t)y0 * w + x1] * tx;
                float b = v[(size_t)y1 * w + x0] * (1 - tx) + v[(size_t)y1 * w + x1] * tx;
                return a * (1 - ty) + b * ty;
            };
            int r8, g8, b8; unpremul(&img.px[((size_t)Y * img.w + X) * 4], r8, g8, b8);
            float q = bil(A[0]) * (r8 / 255.f) + bil(A[1]) * (g8 / 255.f) + bil(A[2]) * (b8 / 255.f) + bil(B);
            out[(size_t)y * rw + x] = std::clamp(q, 0.f, 1.f);
        }
    }
}

void refineEdge(Mask8& m, const Image8& img, const RefineParams& p) {
    if (m.empty() || img.empty()) return;
    Segmenter seg;
    seg.prepare(img, 1200);
    std::vector<float> pw;
    maskToWork(m, seg.ww, seg.wh, pw);
    if (p.smooth > 0.f) {
        int sr = std::max(1, (int)std::lround(p.smooth * 4.f));
        boxBlur(pw, seg.ww, seg.wh, sr);
        for (float& v : pw) v = v > 0.5f ? 1.f : 0.f;
    }
    int r = std::max(1, (int)std::lround(p.radius / seg.scale));
    int x0, y0, x1, y1;
    if (!maskBounds(m, x0, y0, x1, y1)) return;
    int pad = (int)(p.radius * 3 + p.feather * 2 + 8);
    x0 = std::max(0, x0 - pad); y0 = std::max(0, y0 - pad);
    x1 = std::min(m.w, x1 + pad); y1 = std::min(m.h, y1 + pad);
    std::vector<float> q;
    guidedUpsample(seg, img, pw, r, 1e-4f, x0, y0, x1, y1, q);
    float k = 1.f + p.contrast * 6.f;
    int rw = x1 - x0;
    std::fill(m.v.begin(), m.v.end(), 0);
    for (int y = y0; y < y1; ++y) for (int x = x0; x < x1; ++x) {
        float a = q[(size_t)(y - y0) * rw + (x - x0)] + p.shift * 0.35f;
        a = std::clamp((a - 0.5f) * k + 0.5f, 0.f, 1.f);
        m.v[(size_t)y * m.w + x] = (uint8_t)std::lround(a * 255.f);
    }
    featherMask(m, p.feather);
}

// ------------------------------------------------------------------ RLE

std::vector<uint8_t> rleEncode(const Mask8& m) {
    std::vector<uint8_t> out;
    size_t n = m.v.size(), i = 0;
    while (i < n) {
        uint8_t v = m.v[i];
        size_t j = i + 1;
        while (j < n && m.v[j] == v && j - i < 255) ++j;
        out.push_back((uint8_t)(j - i));
        out.push_back(v);
        i = j;
    }
    return out;
}

void rleDecode(const std::vector<uint8_t>& data, Mask8& m, int w, int h) {
    m.reset(w, h, 0);
    size_t k = 0;
    for (size_t i = 0; i + 1 < data.size() && k < m.v.size(); i += 2) {
        size_t cnt = data[i];
        std::fill_n(m.v.begin() + k, std::min(cnt, m.v.size() - k), data[i + 1]);
        k += cnt;
    }
}

} // namespace bp
