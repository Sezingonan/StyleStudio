#include <chrono>
#include <cstdio>
#include <string>
#include <vector>

#include "algorithms/BinarySearch.h"
#include "algorithms/MergeSort.h"
#include "algorithms/QuickSort.h"
#include "managers/CatalogQuery.h"  // detail::PriceLess
#include "managers/ClothingManager.h"
#include "managers/RecommendationManager.h"
#include "storage/SampleData.h"
#include "structures/AVLTree.h"
#include "structures/Trie.h"
#include "text/Normalize.h"

using namespace style;
using Clock = std::chrono::steady_clock;

// Derleyici sonucu kullanılmayan işi silmesin diye sonuçlar buraya yazılır.
static volatile std::size_t sink = 0;

template <typename F>
static double timeMs(F f) {
    const auto t0 = Clock::now();
    f();
    return std::chrono::duration<double, std::milli>(Clock::now() - t0).count();
}

int main() {
    const std::size_t sizes[] = {100, 500, 1000, 5000};
    const std::size_t kMaxGraphN = 1000;  // O(n^2) kenar: 5000'de çok yavaş

    std::printf("%-6s | %-26s | %12s\n", "n", "Islem", "Sure");
    std::printf("-------+----------------------------+-------------\n");

    for (std::size_t n : sizes) {
        ClothingCollection coll = generateSampleClothes(n);
        const std::vector<Clothing> items = coll.all();

        auto row = [n](const char* name, double value, const char* unit) {
            std::printf("%-6zu | %-26s | %9.3f %s\n", n, name, value, unit);
        };

        // ---- AVL ----
        AVLTree<int, Clothing> tree;
        row("AVL insert (toplam)", timeMs([&] { for (const Clothing& c : items) tree.insert(c.id, c); }), "ms");
        row("AVL search (toplam)", timeMs([&] {
                std::size_t f = 0;
                for (const Clothing& c : items) f += tree.find(c.id) != nullptr;
                sink = f;
            }), "ms");
        row("Dizide dogrusal ara (top.)", timeMs([&] {
                std::size_t f = 0;
                for (const Clothing& c : items)
                    for (const Clothing& x : items)
                        if (x.id == c.id) { ++f; break; }
                sink = f;
            }), "ms");
        row("AVL delete (toplam)", timeMs([&] { for (const Clothing& c : items) tree.erase(c.id); }), "ms");

        // ---- Trie ----
        Trie trie;
        row("Trie insert (toplam)", timeMs([&] { for (const Clothing& c : items) trie.insert(c.name, c.id); }), "ms");
        const int sugReps = 1000;
        row("Trie suggest('be', 10)", 1000.0 * timeMs([&] {
                std::size_t f = 0;
                for (int i = 0; i < sugReps; ++i) f += trie.suggest("be", 10).size();
                sink = f;
            }) / sugReps, "us");

        // ---- Sıralamalar ----
        std::vector<Clothing> v = items;
        auto priceLess = [](const Clothing& a, const Clothing& b) { return a.price < b.price; };
        row("Merge Sort (fiyat)", timeMs([&] { mergeSort(v, priceLess); }), "ms");
        v = items;
        row("Quick Sort (fiyat)", timeMs([&] { quickSort(v, priceLess); }), "ms");
        v = items;
        row("Merge Sort (ad, normalize)", timeMs([&] {
                mergeSort(v, [](const Clothing& a, const Clothing& b) {
                    return normalize(a.name) < normalize(b.name);
                });
            }), "ms");

        // ---- Binary Search (fiyata göre sıralı dizide) ----
        v = items;
        quickSort(v, priceLess);
        const int bsReps = 100000;
        row("Binary Search (lowerBound)", 1e6 * timeMs([&] {
                std::size_t f = 0;
                for (int i = 0; i < bsReps; ++i)
                    f += lowerBound(v, v[static_cast<std::size_t>(i) % v.size()].price,
                                    detail::PriceLess{});
                sink = f;
            }) / bsReps, "ns");

        // ---- Graf + BFS ----
        if (n <= kMaxGraphN) {
            ClothingManager mgr;
            mgr.loadFrom(coll);
            RecommendationManager rec(mgr);
            row("Graf kurma (rebuild)", timeMs([&] { rec.rebuild(); }), "ms");
            const int bfsReps = 20;
            row("BFS (tum graf)", timeMs([&] {
                    std::size_t f = 0;
                    for (int i = 0; i < bfsReps; ++i) f += rec.graph().bfs(items[0].id).size();
                    sink = f;
                }) / bfsReps, "ms");
            std::printf("%-6zu | %-26s | %12zu\n", n, "Graf kenar sayisi", rec.graph().edgeCount());
        } else {
            std::printf("%-6zu | %-26s | %12s\n", n, "Graf kurma / BFS", "atlandi");
        }
        std::printf("-------+----------------------------+-------------\n");
    }
    return 0;
}