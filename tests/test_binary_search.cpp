#include <algorithm>
#include <vector>

#include "test_framework.h"
#include "algorithms/BinarySearch.h"
#include "algorithms/QuickSort.h"
#include "models/Clothing.h"

using namespace style;

static Clothing makeClothing(int id, double price) {
    Clothing c;
    c.id = id;
    c.name = "x";
    c.price = price;
    return c;
}

// Clothing ile double'ı her iki yönde karşılaştıran fiyat karşılaştırıcısı.
struct PriceLess {
    bool operator()(const Clothing& a, const Clothing& b) const { return a.price < b.price; }
    bool operator()(const Clothing& a, double p) const { return a.price < p; }
    bool operator()(double p, const Clothing& a) const { return p < a.price; }
};

struct IntLess {
    bool operator()(int a, int b) const { return a < b; }
};

int main() {
    IntLess less;

    // Boş dizi
    {
        std::vector<int> v;
        CHECK_EQ(lowerBound(v, 5, less), 0u);
        CHECK_EQ(upperBound(v, 5, less), 0u);
        CHECK_EQ(binarySearch(v, 5, less), 0u);  // size() == 0: bulunamadı
        CHECK(isSorted(v, less));
    }

    // Temel arama
    {
        std::vector<int> v = {1, 3, 5, 7, 9};
        CHECK_EQ(binarySearch(v, 1, less), 0u);
        CHECK_EQ(binarySearch(v, 5, less), 2u);
        CHECK_EQ(binarySearch(v, 9, less), 4u);
        CHECK_EQ(binarySearch(v, 4, less), v.size());   // yok
        CHECK_EQ(binarySearch(v, 0, less), v.size());   // aralığın altı
        CHECK_EQ(binarySearch(v, 10, less), v.size());  // aralığın üstü
    }

    // Tekrarlı değerlerde sınırlar
    {
        std::vector<int> v = {1, 2, 2, 2, 3};
        CHECK_EQ(lowerBound(v, 2, less), 1u);
        CHECK_EQ(upperBound(v, 2, less), 4u);
        CHECK_EQ(binarySearch(v, 2, less), 1u);  // ilk eşit eleman
        CHECK_EQ(lowerBound(v, 0, less), 0u);
        CHECK_EQ(upperBound(v, 9, less), 5u);
        CHECK_EQ(lowerBound(v, 4, less), 5u);
    }

    // Tek eleman
    {
        std::vector<int> v = {4};
        CHECK_EQ(binarySearch(v, 4, less), 0u);
        CHECK_EQ(binarySearch(v, 3, less), 1u);
        CHECK_EQ(lowerBound(v, 5, less), 1u);
    }

    // isSorted
    {
        CHECK(isSorted(std::vector<int>{1, 2, 2, 3}, less));
        CHECK(!isSorted(std::vector<int>{1, 3, 2}, less));
        CHECK(isSorted(std::vector<int>{7}, less));
    }

    // std::lower_bound / upper_bound ile karşılaştırma (rastgele)
    {
        std::vector<int> v;
        unsigned s = 99;
        for (int i = 0; i < 500; ++i) {
            s = s * 1664525u + 1013904223u;
            v.push_back(static_cast<int>((s >> 8) % 100));
        }
        std::sort(v.begin(), v.end());
        bool ok = true;
        for (int x = -2; x <= 102; ++x) {
            if (lowerBound(v, x, less) !=
                static_cast<std::size_t>(std::lower_bound(v.begin(), v.end(), x) - v.begin()))
                ok = false;
            if (upperBound(v, x, less) !=
                static_cast<std::size_t>(std::upper_bound(v.begin(), v.end(), x) - v.begin()))
                ok = false;
        }
        CHECK(ok);
    }

    // Fiyat aralığı filtresi: [min, max]
    {
        std::vector<Clothing> v = {makeClothing(1, 900), makeClothing(2, 300),
                                   makeClothing(3, 2100), makeClothing(4, 850),
                                   makeClothing(5, 1200), makeClothing(6, 850)};
        quickSort(v, PriceLess{});
        CHECK(isSorted(v, PriceLess{}));

        const std::size_t first = lowerBound(v, 500.0, PriceLess{});
        const std::size_t last = upperBound(v, 1200.0, PriceLess{});  // 1200 dahil
        CHECK_EQ(last - first, 4u);  // 850, 850, 900, 1200
        for (std::size_t i = first; i < last; ++i) {
            CHECK(v[i].price >= 500.0 && v[i].price <= 1200.0);
        }

        // Hiç ürün yok: aralık boş
        CHECK_EQ(lowerBound(v, 5000.0, PriceLess{}), upperBound(v, 6000.0, PriceLess{}));
        // Tam fiyat araması
        const std::size_t pos = binarySearch(v, 2100.0, PriceLess{});
        CHECK(pos < v.size());
        if (pos < v.size()) CHECK_EQ(v[pos].id, 3);
        CHECK_EQ(binarySearch(v, 777.0, PriceLess{}), v.size());
    }

    return finish("test_binary_search");
}