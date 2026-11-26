#include <vector>

#include "test_framework.h"
#include "structures/Graph.h"

using namespace style;

static std::vector<int> idsOf(const std::vector<Graph::Visit>& v) {
    std::vector<int> out;
    for (const auto& x : v) out.push_back(x.id);
    return out;
}

// Düğüm ziyaret edilmediyse -1.
static int levelOf(const std::vector<Graph::Visit>& v, int id) {
    for (const auto& x : v) if (x.id == id) return x.level;
    return -1;
}

using Ids = std::vector<int>;

int main() {
    // Boş graf
    {
        Graph g;
        CHECK_EQ(g.nodeCount(), 0u);
        CHECK_EQ(g.edgeCount(), 0u);
        CHECK(g.bfs(1).empty());          // olmayan başlangıç
        CHECK(g.neighbors(1) == nullptr);
        CHECK(!g.removeNode(1));
        CHECK(!g.addEdge(1, 2));          // düğümler yok
    }

    // Düğüm ve kenar kuralları
    {
        Graph g;
        CHECK(g.addNode(1));
        CHECK(!g.addNode(1));             // tekrar
        g.addNode(2);
        CHECK(g.addEdge(1, 2));
        CHECK(!g.addEdge(1, 2));          // tekrarlı kenar
        CHECK(!g.addEdge(2, 1));          // ters yön de aynı kenar
        CHECK(!g.addEdge(1, 1));          // kendine kenar
        CHECK(!g.addEdge(1, 99));         // olmayan düğüm
        CHECK_EQ(g.edgeCount(), 1u);
        CHECK(g.hasEdge(1, 2));
        CHECK(g.hasEdge(2, 1));           // yönsüz
        CHECK(!g.hasEdge(1, 99));
    }

    // Kenar ve düğüm silme
    {
        Graph g;
        for (int i = 1; i <= 4; ++i) g.addNode(i);
        g.addEdge(1, 2); g.addEdge(1, 3); g.addEdge(1, 4); g.addEdge(2, 3);
        CHECK_EQ(g.edgeCount(), 4u);
        CHECK(g.removeEdge(2, 3));
        CHECK(!g.removeEdge(2, 3));       // ikinci kez olmaz
        CHECK_EQ(g.edgeCount(), 3u);
        CHECK(g.removeNode(1));           // 3 kenarı birden gider
        CHECK_EQ(g.edgeCount(), 0u);
        CHECK_EQ(g.nodeCount(), 3u);
        CHECK(!g.hasEdge(2, 1));
        CHECK(g.neighbors(2)->empty());   // komşu listelerinde kalıntı yok
        CHECK(!g.hasNode(1));
    }

    // BFS: ziyaret sırası ve seviyeler
    // 1-2, 2-3, 2-5, 3-4
    {
        Graph g;
        for (int i = 1; i <= 5; ++i) g.addNode(i);
        g.addEdge(1, 2); g.addEdge(2, 3); g.addEdge(2, 5); g.addEdge(3, 4);
        auto v = g.bfs(1);
        CHECK((idsOf(v) == Ids{1, 2, 3, 5, 4}));
        CHECK_EQ(levelOf(v, 1), 0);
        CHECK_EQ(levelOf(v, 2), 1);
        CHECK_EQ(levelOf(v, 3), 2);
        CHECK_EQ(levelOf(v, 5), 2);
        CHECK_EQ(levelOf(v, 4), 3);
    }

    // BFS: maxLevel sınırı
    {
        Graph g;
        for (int i = 1; i <= 4; ++i) g.addNode(i);
        g.addEdge(1, 2); g.addEdge(2, 3); g.addEdge(3, 4);
        CHECK((idsOf(g.bfs(1, 0)) == Ids{1}));
        CHECK((idsOf(g.bfs(1, 1)) == Ids{1, 2}));
        CHECK((idsOf(g.bfs(1, 2)) == Ids{1, 2, 3}));
        CHECK((idsOf(g.bfs(1, -1)) == Ids{1, 2, 3, 4}));  // sınırsız
    }

    // BFS: ulaşılamayan düğüm
    {
        Graph g;
        for (int i = 1; i <= 3; ++i) g.addNode(i);
        g.addEdge(1, 2);  // 3 yalnız
        auto v = g.bfs(1);
        CHECK_EQ(v.size(), 2u);
        CHECK_EQ(levelOf(v, 3), -1);
        CHECK_EQ(g.bfs(3).size(), 1u);    // yalnız düğümden sadece kendisi
    }

    // BFS: döngülü graf, her düğüm bir kez ziyaret edilir (elmas: 1-2,1-3,2-4,3-4)
    {
        Graph g;
        for (int i = 1; i <= 4; ++i) g.addNode(i);
        g.addEdge(1, 2); g.addEdge(1, 3); g.addEdge(2, 4); g.addEdge(3, 4);
        auto v = g.bfs(1);
        CHECK_EQ(v.size(), 4u);
        CHECK_EQ(levelOf(v, 4), 2);       // en kısa yol 2 kenar
    }

    // BFS: üçgen (tekrar ziyaret kontrolü)
    {
        Graph g;
        for (int i = 1; i <= 3; ++i) g.addNode(i);
        g.addEdge(1, 2); g.addEdge(2, 3); g.addEdge(3, 1);
        CHECK_EQ(g.bfs(2).size(), 3u);
    }

    // BFS: silinen düğüm artık ziyaret edilmemeli
    {
        Graph g;
        for (int i = 1; i <= 3; ++i) g.addNode(i);
        g.addEdge(1, 2); g.addEdge(2, 3);
        g.removeNode(2);
        CHECK((idsOf(g.bfs(1)) == Ids{1}));
    }

    // Büyük zincir: 1000 düğüm, son düğümün seviyesi 999
    {
        Graph g;
        for (int i = 1; i <= 1000; ++i) g.addNode(i);
        for (int i = 1; i < 1000; ++i) g.addEdge(i, i + 1);
        auto v = g.bfs(1);
        CHECK_EQ(v.size(), 1000u);
        CHECK_EQ(levelOf(v, 1000), 999);
    }

    return finish("test_graph");
}