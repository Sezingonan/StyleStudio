#include <algorithm>
#include <string>
#include <utility>
#include <vector>

#include "test_framework.h"
#include "algorithms/MergeSort.h"
#include "models/Clothing.h"
#include "text/Normalize.h"

using namespace style;

static bool intLess(int a, int b) { return a < b; }

// Sabit tohumlu basit sözde rastgele sayı üretici (testler tekrarlanabilir olsun).
static unsigned next(unsigned& s) {
    s = s * 1664525u + 1013904223u;
    return s >> 8;
}

int main() {
    // Boş ve tek elemanlı dizi
    {
        std::vector<int> v;
        mergeSort(v, intLess);
        CHECK(v.empty());
        std::vector<int> one = {5};
        mergeSort(one, intLess);
        CHECK((one == std::vector<int>{5}));
    }

    // Sıralı, ters ve tekrarlı girdiler
    {
        std::vector<int> a = {1, 2, 3, 4, 5};
        mergeSort(a, intLess);
        CHECK((a == std::vector<int>{1, 2, 3, 4, 5}));

        std::vector<int> b = {5, 4, 3, 2, 1};
        mergeSort(b, intLess);
        CHECK((b == std::vector<int>{1, 2, 3, 4, 5}));

        std::vector<int> c = {3, 1, 3, 2, 1, 3};
        mergeSort(c, intLess);
        CHECK((c == std::vector<int>{1, 1, 2, 3, 3, 3}));

        std::vector<int> d = {7, 7, 7, 7};
        mergeSort(d, intLess);
        CHECK((d == std::vector<int>{7, 7, 7, 7}));
    }

    // Azalan sıralama (karşılaştırıcıyı ters çevirerek)
    {
        std::vector<int> v = {2, 9, 4, 1};
        mergeSort(v, [](int a, int b) { return a > b; });
        CHECK((v == std::vector<int>{9, 4, 2, 1}));
    }

    // Kararlılık: aynı anahtarlılar eski sırasını korumalı
    {
        std::vector<std::pair<int, int>> v = {{2, 0}, {1, 1}, {2, 2}, {1, 3}, {2, 4}};
        mergeSort(v, [](const std::pair<int, int>& x, const std::pair<int, int>& y) {
            return x.first < y.first;
        });
        const std::vector<std::pair<int, int>> expected = {{1, 1}, {1, 3}, {2, 0}, {2, 2}, {2, 4}};
        CHECK(v == expected);
    }

    // 1000 rastgele sayı: std::sort ile aynı sonucu vermeli
    {
        unsigned seed = 12345;
        std::vector<int> a;
        for (int i = 0; i < 1000; ++i) a.push_back(static_cast<int>(next(seed) % 500));
        std::vector<int> expected = a;
        std::sort(expected.begin(), expected.end());
        mergeSort(a, intLess);
        CHECK(a == expected);
    }

    // Ürünleri normalize edilmiş ada göre sırala (Türkçe harfler dahil)
    {
        auto make = [](int id, const char* name) {
            Clothing c;
            c.id = id;
            c.name = name;
            c.price = 1;
            return c;
        };
        std::vector<Clothing> v = {make(1, "Tişört"), make(2, "Bot"), make(3, "Çanta"),
                                   make(4, "Aksesuar")};
        mergeSort(v, [](const Clothing& a, const Clothing& b) {
            return normalize(a.name) < normalize(b.name);
        });
        // aksesuar < bot < canta < tisort
        CHECK_EQ(v[0].id, 4);
        CHECK_EQ(v[1].id, 2);
        CHECK_EQ(v[2].id, 3);
        CHECK_EQ(v[3].id, 1);
    }

    return finish("test_sort");
}