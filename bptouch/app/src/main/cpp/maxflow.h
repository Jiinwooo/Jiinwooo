// SPDX-License-Identifier: MIT OR Apache-2.0
// BP Touch — max-flow/min-cut para grafos de imagem.
// Implementação própria do algoritmo de Boykov & Kolmogorov (2004), "An Experimental Comparison
// of Min-Cut/Max-Flow Algorithms for Energy Minimization in Vision": duas árvores de busca
// (fonte e sorvedouro) que crescem, aumentam o fluxo e adotam órfãos.
#pragma once
#include <cstdint>
#include <vector>
#include <deque>

namespace bp {

class MaxFlow {
public:
    using cap_t = int32_t;

    explicit MaxFlow(int nodeHint = 0, int edgeHint = 0);
    int addNodes(int n);                     // devolve o índice do primeiro
    // Capacidades terminais (somam com as anteriores do mesmo nó)
    void addTWeights(int i, cap_t toSource, cap_t toSink);
    // Aresta i→j com capacidade cap e j→i com revCap
    void addEdge(int i, int j, cap_t cap, cap_t revCap);
    int64_t solve();
    // true = lado da fonte (primeiro plano)
    bool isSource(int i) const;
    int nodeCount() const { return (int)nodes_.size(); }

private:
    static constexpr int NONE = -1;
    static constexpr int TERMINAL = -2;
    static constexpr int ORPHAN = -3;

    struct Node {
        int first = NONE;       // primeira aresta saindo
        int parent = NONE;      // aresta para o pai (NONE = livre)
        int nextActive = NONE;  // fila de ativos (encadeada); o próprio índice marca o fim
        int ts = 0;             // carimbo de tempo da distância
        int dist = 0;           // distância até o terminal
        bool sink = false;      // pertence à árvore do sorvedouro
        cap_t trCap = 0;        // >0: residual da fonte; <0: residual para o sorvedouro
    };
    struct Arc {
        int head;       // nó de destino
        int next;       // próxima aresta do mesmo nó de origem
        cap_t rCap;     // capacidade residual
    };
    // A aresta irmã (reversa) de a é a ^ 1.

    void setActive(int i);
    int nextActive();
    void augment(int middle);
    void processSourceOrphan(int i);
    void processSinkOrphan(int i);

    std::vector<Node> nodes_;
    std::vector<Arc> arcs_;
    int qFirst_ = NONE, qLast_ = NONE;
    std::deque<int> orphans_;
    int time_ = 0;
    int64_t flow_ = 0;
};

} // namespace bp
