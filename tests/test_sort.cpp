#include <algorithm>
#include <string>
#include <utility>
#include <vector>

#include "test_framework.h"
#include "algorithms/MergeSort.h"
#include "algorithms/QuickSort.h"
#include "models/Clothing.h"
#include "text/Normalize.h"

using namespace style;

static bool intLess(int a, int b) { return a < b; }

// Sabit tohumlu basit sözde rastgele sayı üretici (testler tekrarlanabilir olsun).
static unsigned next(unsigned& s) {
    s = s * 1664525u + 1013904223u;
    return s >> 8;
}

static Clothing makeClothing(int id, const char* name, double price) {
    Clothing c;
    c.id = id;
    c.name = name;
    c.price = price;
    return c;
}

int main() {
    // ---------- Merge Sort ----------

    {
        std::vector<int> v;
        mergeSort(v, intLess);
        CHECK(v.empty());
        std::vector<int> one = {5};
        mergeSort(one, intLess);
        CHECK((one == std::vector<int>{5}));
    }

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

    {
        std::vector<int> v = {2, 9, 4, 1};
        mergeSort(v, [](int a, int b) { return a > b; });
        CHECK((v == std::vector<int>{9, 4, 2, 1}));
    }

    // Kararlılık
    {
        std::vector<std::pair<int, int>> v = {{2, 0}, {1, 1}, {2, 2}, {1, 3}, {2, 4}};
        mergeSort(v, [](const std::pair<int, int>& x, const std::pair<int, int>& y) {
            return x.first < y.first;
        });
        const std::vector<std::pair<int, int>> expected = {{1, 1}, {1, 3}, {2, 0}, {2, 2}, {2, 4}};
        CHECK(v == expected);
    }

    {
        unsigned seed = 12345;
        std::vector<int> a;
        for (int i = 0; i < 1000; ++i) a.push_back(static_cast<int>(next(seed) % 500));
        std::vector<int> expected = a;
        std::sort(expected.begin(), expected.end());
        mergeSort(a, intLess);
        CHECK(a == expected);
    }

    // Ada göre sıralama (Türkçe harfler dahil)
    {
        std::vector<Clothing> v = {makeClothing(1, "Tişört", 1), makeClothing(2, "Bot", 1),
                                   makeClothing(3, "Çanta", 1), makeClothing(4, "Aksesuar", 1)};
        mergeSort(v, [](const Clothing& a, const Clothing& b) {
            return normalize(a.name) < normalize(b.name);
        });
        CHECK_EQ(v[0].id, 4);
        CHECK_EQ(v[1].id, 2);
        CHECK_EQ(v[2].id, 3);
        CHECK_EQ(v[3].id, 1);
    }

    // ---------- Quick Sort ----------

    // Boş, tek, iki eleman
    {
        std::vector<int> v;
        quickSort(v, intLess);
        CHECK(v.empty());
        std::vector<int> one = {5};
        quickSort(one, intLess);
        CHECK((one == std::vector<int>{5}));
        std::vector<int> two = {9, 3};
        quickSort(two, intLess);
        CHECK((two == std::vector<int>{3, 9}));
        std::vector<int> two2 = {3, 9};
        quickSort(two2, intLess);
        CHECK((two2 == std::vector<int>{3, 9}));
    }

    // Sıralı, ters, tekrarlı, hepsi aynı
    {
        std::vector<int> a = {1, 2, 3, 4, 5};
        quickSort(a, intLess);
        CHECK((a == std::vector<int>{1, 2, 3, 4, 5}));

        std::vector<int> b = {5, 4, 3, 2, 1};
        quickSort(b, intLess);
        CHECK((b == std::vector<int>{1, 2, 3, 4, 5}));

        std::vector<int> c = {3, 1, 3, 2, 1, 3};
        quickSort(c, intLess);
        CHECK((c == std::vector<int>{1, 1, 2, 3, 3, 3}));

        std::vector<int> d = {7, 7, 7, 7};
        quickSort(d, intLess);
        CHECK((d == std::vector<int>{7, 7, 7, 7}));
    }

    // Azalan sıralama
    {
        std::vector<int> v = {2, 9, 4, 1};
        quickSort(v, [](int a, int b) { return a > b; });
        CHECK((v == std::vector<int>{9, 4, 2, 1}));
    }

    // Rastgele: std::sort ile aynı sonucu vermeli (farklı tohum ve boyutlar)
    {
        bool allMatch = true;
        for (unsigned seed0 = 1; seed0 <= 50; ++seed0) {
            unsigned seed = seed0 * 7919u;
            const int n = static_cast<int>(next(seed) % 200);
            std::vector<int> a;
            for (int i = 0; i < n; ++i) a.push_back(static_cast<int>(next(seed) % 50));
            std::vector<int> expected = a;
            std::sort(expected.begin(), expected.end());
            quickSort(a, intLess);
            if (a != expected) allMatch = false;
        }
        CHECK(allMatch);
    }

    // Büyük girdiler: sıralı, ters ve çok tekrarlı (naif pivotta O(n^2) olan durumlar)
    {
        std::vector<int> asc, desc, dup;
        for (int i = 0; i < 20000; ++i) {
            asc.push_back(i);
            desc.push_back(19999 - i);
            dup.push_back(i % 3);
        }
        std::vector<int> e1 = asc, e2 = asc, e3 = dup;
        std::sort(e3.begin(), e3.end());
        quickSort(asc, intLess);
        quickSort(desc, intLess);
        quickSort(dup, intLess);
        CHECK(asc == e1);
        CHECK(desc == e2);
        CHECK(dup == e3);
    }

    // Ürünleri fiyata göre sırala
    {
        std::vector<Clothing> v = {makeClothing(1, "A", 900), makeClothing(2, "B", 300),
                                   makeClothing(3, "C", 2100), makeClothing(4, "D", 850)};
        quickSort(v, [](const Clothing& a, const Clothing& b) { return a.price < b.price; });
        CHECK_EQ(v[0].id, 2);
        CHECK_EQ(v[1].id, 4);
        CHECK_EQ(v[2].id, 1);
        CHECK_EQ(v[3].id, 3);
    }

    return finish("test_sort");
}