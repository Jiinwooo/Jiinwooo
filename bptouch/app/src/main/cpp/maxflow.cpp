// SPDX-License-Identifier: MIT OR Apache-2.0
#include "maxflow.h"
#include <algorithm>
#include <climits>

namespace bp {

MaxFlow::MaxFlow(int nodeHint, int edgeHint) {
    nodes_.reserve(nodeHint);
    arcs_.reserve((size_t)edgeHint * 2);
}

int MaxFlow::addNodes(int n) {
    int first = (int)nodes_.size();
    nodes_.resize(nodes_.size() + n);
    return first;
}

void MaxFlow::addTWeights(int i, cap_t toSource, cap_t toSink) {
    cap_t delta = nodes_[i].trCap;
    if (delta > 0) toSource += delta; else toSink -= delta;
    flow_ += std::min(toSource, toSink);
    nodes_[i].trCap = toSource - toSink;
}

void MaxFlow::addEdge(int i, int j, cap_t cap, cap_t revCap) {
    int a = (int)arcs_.size();
    arcs_.push_back({j, nodes_[i].first, cap});
    arcs_.push_back({i, nodes_[j].first, revCap});
    nodes_[i].first = a;
    nodes_[j].first = a + 1;
}

void MaxFlow::setActive(int i) {
    Node& n = nodes_[i];
    if (n.nextActive != NONE) return;          // já está na fila
    if (qLast_ == NONE) qFirst_ = i; else nodes_[qLast_].nextActive = i;
    qLast_ = i;
    n.nextActive = i;                          // fim da fila aponta para si
}

int MaxFlow::nextActive() {
    while (qFirst_ != NONE) {
        int i = qFirst_;
        Node& n = nodes_[i];
        qFirst_ = (n.nextActive == i) ? NONE : n.nextActive;
        if (qFirst_ == NONE) qLast_ = NONE;
        n.nextActive = NONE;
        if (n.parent != NONE) return i;        // ignora nós que ficaram livres
    }
    return NONE;
}

bool MaxFlow::isSource(int i) const {
    const Node& n = nodes_[i];
    return n.parent != NONE && !n.sink;
}

void MaxFlow::augment(int middle) {
    // middle: aresta s-tree → t-tree. Origem = head da irmã.
    int sNode = arcs_[middle ^ 1].head;
    int tNode = arcs_[middle].head;

    cap_t b = arcs_[middle].rCap;
    for (int i = sNode;;) {
        int a = nodes_[i].parent;
        if (a == TERMINAL) { b = std::min(b, nodes_[i].trCap); break; }
        b = std::min(b, arcs_[a ^ 1].rCap);
        i = arcs_[a].head;
    }
    for (int i = tNode;;) {
        int a = nodes_[i].parent;
        if (a == TERMINAL) { b = std::min(b, -nodes_[i].trCap); break; }
        b = std::min(b, arcs_[a].rCap);
        i = arcs_[a].head;
    }

    arcs_[middle ^ 1].rCap += b;
    arcs_[middle].rCap -= b;
    for (int i = sNode;;) {
        int a = nodes_[i].parent;
        if (a == TERMINAL) {
            nodes_[i].trCap -= b;
            if (!nodes_[i].trCap) { nodes_[i].parent = ORPHAN; orphans_.push_front(i); }
            break;
        }
        arcs_[a].rCap += b;
        arcs_[a ^ 1].rCap -= b;
        int up = arcs_[a].head;
        if (!arcs_[a ^ 1].rCap) { nodes_[i].parent = ORPHAN; orphans_.push_front(i); }
        i = up;
    }
    for (int i = tNode;;) {
        int a = nodes_[i].parent;
        if (a == TERMINAL) {
            nodes_[i].trCap += b;
            if (!nodes_[i].trCap) { nodes_[i].parent = ORPHAN; orphans_.push_front(i); }
            break;
        }
        arcs_[a ^ 1].rCap += b;
        arcs_[a].rCap -= b;
        int up = arcs_[a].head;
        if (!arcs_[a].rCap) { nodes_[i].parent = ORPHAN; orphans_.push_front(i); }
        i = up;
    }
    flow_ += b;
}

void MaxFlow::processSourceOrphan(int i) {
    int bestArc = NONE, bestDist = INT_MAX;
    for (int a0 = nodes_[i].first; a0 != NONE; a0 = arcs_[a0].next) {
        if (!arcs_[a0 ^ 1].rCap) continue;          // precisa de residual j→i
        int j = arcs_[a0].head;
        if (nodes_[j].sink || nodes_[j].parent == NONE) continue;
        // mede a distância de j até a fonte, conferindo se a cadeia chega ao terminal
        int d = 0, k = j;
        bool ok = false;
        for (;;) {
            if (nodes_[k].ts == time_) { d += nodes_[k].dist; ok = true; break; }
            int a = nodes_[k].parent;
            d++;
            if (a == TERMINAL) { nodes_[k].ts = time_; nodes_[k].dist = 1; ok = true; break; }
            if (a == ORPHAN || a == NONE) break;
            k = arcs_[a].head;
        }
        if (!ok) continue;
        if (d < bestDist) { bestArc = a0; bestDist = d; }
        // atualiza carimbos ao longo do caminho
        for (int k2 = j; nodes_[k2].ts != time_; k2 = arcs_[nodes_[k2].parent].head) {
            nodes_[k2].ts = time_;
            nodes_[k2].dist = d--;
        }
    }
    if (bestArc != NONE) {
        nodes_[i].parent = bestArc;
        nodes_[i].ts = time_;
        nodes_[i].dist = bestDist + 1;
        return;
    }
    nodes_[i].parent = NONE;   // vira livre
    for (int a0 = nodes_[i].first; a0 != NONE; a0 = arcs_[a0].next) {
        int j = arcs_[a0].head;
        if (nodes_[j].sink) continue;
        int a = nodes_[j].parent;
        if (a == NONE) continue;
        if (arcs_[a0 ^ 1].rCap) setActive(j);
        if (a != TERMINAL && a != ORPHAN && arcs_[a].head == i) {
            nodes_[j].parent = ORPHAN;
            orphans_.push_back(j);
        }
    }
}

void MaxFlow::processSinkOrphan(int i) {
    int bestArc = NONE, bestDist = INT_MAX;
    for (int a0 = nodes_[i].first; a0 != NONE; a0 = arcs_[a0].next) {
        if (!arcs_[a0].rCap) continue;              // precisa de residual i→j
        int j = arcs_[a0].head;
        if (!nodes_[j].sink || nodes_[j].parent == NONE) continue;
        int d = 0, k = j;
        bool ok = false;
        for (;;) {
            if (nodes_[k].ts == time_) { d += nodes_[k].dist; ok = true; break; }
            int a = nodes_[k].parent;
            d++;
            if (a == TERMINAL) { nodes_[k].ts = time_; nodes_[k].dist = 1; ok = true; break; }
            if (a == ORPHAN || a == NONE) break;
            k = arcs_[a].head;
        }
        if (!ok) continue;
        if (d < bestDist) { bestArc = a0; bestDist = d; }
        for (int k2 = j; nodes_[k2].ts != time_; k2 = arcs_[nodes_[k2].parent].head) {
            nodes_[k2].ts = time_;
            nodes_[k2].dist = d--;
        }
    }
    if (bestArc != NONE) {
        nodes_[i].parent = bestArc;
        nodes_[i].ts = time_;
        nodes_[i].dist = bestDist + 1;
        return;
    }
    nodes_[i].parent = NONE;
    for (int a0 = nodes_[i].first; a0 != NONE; a0 = arcs_[a0].next) {
        int j = arcs_[a0].head;
        if (!nodes_[j].sink) continue;
        int a = nodes_[j].parent;
        if (a == NONE) continue;
        if (arcs_[a0].rCap) setActive(j);
        if (a != TERMINAL && a != ORPHAN && arcs_[a].head == i) {
            nodes_[j].parent = ORPHAN;
            orphans_.push_back(j);
        }
    }
}

int64_t MaxFlow::solve() {
    // inicialização: nós com capacidade terminal entram nas árvores
    qFirst_ = qLast_ = NONE;
    for (int i = 0; i < (int)nodes_.size(); ++i) {
        Node& n = nodes_[i];
        n.nextActive = NONE;
        n.ts = 0;
        if (n.trCap > 0) { n.sink = false; n.parent = TERMINAL; n.dist = 1; setActive(i); }
        else if (n.trCap < 0) { n.sink = true; n.parent = TERMINAL; n.dist = 1; setActive(i); }
        else n.parent = NONE;
    }
    time_ = 0;

    int current = NONE;
    for (;;) {
        int i = current;
        if (i != NONE) {
            nodes_[i].nextActive = NONE;
            if (nodes_[i].parent == NONE) i = NONE;
        }
        if (i == NONE) {
            i = nextActive();
            if (i == NONE) break;
        }

        // crescimento
        int found = NONE;
        if (!nodes_[i].sink) {
            for (int a = nodes_[i].first; a != NONE; a = arcs_[a].next) {
                if (!arcs_[a].rCap) continue;
                int j = arcs_[a].head;
                Node& nj = nodes_[j];
                if (nj.parent == NONE) {
                    nj.sink = false; nj.parent = a ^ 1; nj.ts = nodes_[i].ts; nj.dist = nodes_[i].dist + 1;
                    setActive(j);
                } else if (nj.sink) { found = a; break; }
                else if (nj.ts <= nodes_[i].ts && nj.dist > nodes_[i].dist) {
                    nj.parent = a ^ 1; nj.ts = nodes_[i].ts; nj.dist = nodes_[i].dist + 1;
                }
            }
        } else {
            for (int a = nodes_[i].first; a != NONE; a = arcs_[a].next) {
                if (!arcs_[a ^ 1].rCap) continue;
                int j = arcs_[a].head;
                Node& nj = nodes_[j];
                if (nj.parent == NONE) {
                    nj.sink = true; nj.parent = a ^ 1; nj.ts = nodes_[i].ts; nj.dist = nodes_[i].dist + 1;
                    setActive(j);
                } else if (!nj.sink) { found = a ^ 1; break; }
                else if (nj.ts <= nodes_[i].ts && nj.dist > nodes_[i].dist) {
                    nj.parent = a ^ 1; nj.ts = nodes_[i].ts; nj.dist = nodes_[i].dist + 1;
                }
            }
        }

        time_++;
        if (found != NONE) {
            nodes_[i].nextActive = i;   // continua ativo nesta rodada
            current = i;
            augment(found);
            while (!orphans_.empty()) {
                int o = orphans_.front(); orphans_.pop_front();
                if (nodes_[o].sink) processSinkOrphan(o); else processSourceOrphan(o);
            }
        } else {
            current = NONE;
        }
    }
    return flow_;
}

} // namespace bp
