#pragma once
#include <optional>
#include <vector>

#include "algorithms/BinarySearch.h"
#include "algorithms/MergeSort.h"
#include "algorithms/QuickSort.h"
#include "managers/ClothingManager.h"
#include "text/Normalize.h"

namespace style {

enum class SortOrder { None, PriceAsc, PriceDesc, NameAsc };

struct CatalogFilter {
    std::optional<Category> category;
    std::optional<Color> color;
    std::optional<Style> style;
    std::optional<Season> season;  // All olan ürünler her mevsimde uyar
    std::optional<double> minPrice;
    std::optional<double> maxPrice;
    SortOrder order = SortOrder::None;
};

namespace detail {

// Fiyat karşılaştırıcısı: Clothing-Clothing ve Clothing-double (Binary Search için).
struct PriceLess {
    bool operator()(const Clothing& a, const Clothing& b) const { return a.price < b.price; }
    bool operator()(const Clothing& a, double p) const { return a.price < p; }
    bool operator()(double p, const Clothing& a) const { return p < a.price; }
};

inline bool matchesAttributes(const Clothing& c, const CatalogFilter& f) {
    if (f.category && c.category != *f.category) return false;
    if (f.color && c.color != *f.color) return false;
    if (f.style && c.style != *f.style) return false;
    if (f.season && c.season != *f.season && c.season != Season::All) return false;
    return true;
}

}  // namespace detail

// Filtreler ve sıralamayla ürün listesi döner.
// Fiyat sınırı varsa: Quick Sort ile fiyata göre sırala, Binary Search ile aralığı bul.
inline std::vector<Clothing> queryCatalog(const ClothingManager& items,
                                          const CatalogFilter& f) {
    std::vector<Clothing> pool = items.all();  // ID sırasıyla (AVL in-order)

    if (f.minPrice || f.maxPrice) {
        const double lo = f.minPrice.value_or(0.0);
        const double hi = f.maxPrice.value_or(1e18);
        if (lo > hi) return {};

        quickSort(pool, detail::PriceLess{});
        const std::size_t first = lowerBound(pool, lo, detail::PriceLess{});
        const std::size_t last = upperBound(pool, hi, detail::PriceLess{});
        pool = std::vector<Clothing>(pool.begin() + static_cast<std::ptrdiff_t>(first),
                                     pool.begin() + static_cast<std::ptrdiff_t>(last));
    }

    std::vector<Clothing> result;
    for (const Clothing& c : pool) {
        if (detail::matchesAttributes(c, f)) result.push_back(c);
    }

    switch (f.order) {
        case SortOrder::PriceAsc:
            quickSort(result, [](const Clothing& a, const Clothing& b) { return a.price < b.price; });
            break;
        case SortOrder::PriceDesc:
            quickSort(result, [](const Clothing& a, const Clothing& b) { return a.price > b.price; });
            break;
        case SortOrder::NameAsc:
            mergeSort(result, [](const Clothing& a, const Clothing& b) {
                return normalize(a.name) < normalize(b.name);
            });
            break;
        case SortOrder::None:
            // Fiyat filtresi pool'u fiyata göre sıraladıysa ID sırasına dön.
            if (f.minPrice || f.maxPrice) {
                mergeSort(result, [](const Clothing& a, const Clothing& b) { return a.id < b.id; });
            }
            break;
    }
    return result;
}

}  // namespace style