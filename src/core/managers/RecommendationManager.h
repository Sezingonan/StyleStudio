#pragma once
#include <cstddef>
#include <vector>

#include "managers/ClothingManager.h"
#include "managers/Compatibility.h"
#include "structures/Graph.h"

namespace style {

// Uyumluluk kurallarından graf üretir, BFS ile seviyeli öneri verir.
// Ürünleri ClothingManager'dan okur (kopya tutmaz); yalnızca graf burada durur.
class RecommendationManager {
public:
    struct Suggestion {
        Clothing item;
        int level;  // 1 = doğrudan uyumlu, 2 = bir ara ürün üzerinden uyumlu, ...
    };

    explicit RecommendationManager(const ClothingManager& items) : items_(items) {}

    // Grafı sıfırdan kurar: tüm çiftler denenir. Zaman: O(n^2) kural kontrolü.
    void rebuild() {
        graph_ = Graph{};
        const std::vector<Clothing> list = items_.all();
        for (const Clothing& c : list) graph_.addNode(c.id);
        for (std::size_t i = 0; i < list.size(); ++i) {
            for (std::size_t j = i + 1; j < list.size(); ++j) {
                if (areCompatible(list[i], list[j])) graph_.addEdge(list[i].id, list[j].id);
            }
        }
    }

    // Ürün ClothingManager'a eklendikten SONRA çağrılır. Düğümü ekler ve
    // mevcut ürünlerle kenar çeker. Ürün yoksa ya da düğüm zaten varsa false.
    bool onAdded(int id) {
        const Clothing* added = items_.findById(id);
        if (!added || !graph_.addNode(id)) return false;
        const Clothing item = *added;  // kopya: forEach sırasında işaretçiye güvenme
        items_.forEach([&](const Clothing& other) {
            if (other.id != id && graph_.hasNode(other.id) && areCompatible(item, other)) {
                graph_.addEdge(id, other.id);
            }
        });
        return true;
    }

    // Ürün ClothingManager'dan silindikten sonra çağrılır.
    bool onRemoved(int id) { return graph_.removeNode(id); }

    // id'ye uyumlu ürünler, BFS sırasıyla (önce seviye 1, sonra 2...).
    // maxLevel >= 1 olmalı. Başlangıçla aynı kategorideki ürünler elenir.
    // En fazla limit öneri döner. Bilinmeyen id ya da maxLevel < 1 ise boş.
    std::vector<Suggestion> recommend(int id, int maxLevel = 2,
                                      std::size_t limit = 20) const {
        std::vector<Suggestion> result;
        const Clothing* start = items_.findById(id);
        if (!start || maxLevel < 1 || !graph_.hasNode(id)) return result;

        for (const Graph::Visit& v : graph_.bfs(id, maxLevel)) {
            if (v.level == 0) continue;  // başlangıç ürünü
            if (result.size() >= limit) break;
            const Clothing* c = items_.findById(v.id);
            if (!c || c->category == start->category) continue;
            result.push_back({*c, v.level});
        }
        return result;
    }

    const Graph& graph() const { return graph_; }

private:
    const ClothingManager& items_;
    Graph graph_;
};

}  // namespace style