#include <vector>

#include "test_framework.h"
#include "structures/AVLTree.h"

using namespace style;

int main() {
    // Boş ağaç
    {
        AVLTree<int, int> t;
        CHECK(t.empty());
        CHECK_EQ(t.size(), 0u);
        CHECK_EQ(t.height(), 0);
        CHECK(t.isBalanced());
        CHECK(t.find(1) == nullptr);   // boş ağaçta arama çökmemeli
        CHECK(!t.contains(1));
    }

    // Sol-Sol: 3, 2, 1 -> sağa rotasyon, yükseklik 2
    {
        AVLTree<int, int> t;
        t.insert(3, 0); t.insert(2, 0); t.insert(1, 0);
        CHECK_EQ(t.height(), 2);
        CHECK(t.isBalanced());
    }

    // Sağ-Sağ: 1, 2, 3 -> sola rotasyon
    {
        AVLTree<int, int> t;
        t.insert(1, 0); t.insert(2, 0); t.insert(3, 0);
        CHECK_EQ(t.height(), 2);
        CHECK(t.isBalanced());
    }

    // Sol-Sağ: 3, 1, 2 -> çift rotasyon
    {
        AVLTree<int, int> t;
        t.insert(3, 0); t.insert(1, 0); t.insert(2, 0);
        CHECK_EQ(t.height(), 2);
        CHECK(t.isBalanced());
    }

    // Sağ-Sol: 1, 3, 2 -> çift rotasyon
    {
        AVLTree<int, int> t;
        t.insert(1, 0); t.insert(3, 0); t.insert(2, 0);
        CHECK_EQ(t.height(), 2);
        CHECK(t.isBalanced());
    }

    // Tekrarlanan anahtar reddedilmeli, eski değer korunmalı
    {
        AVLTree<int, int> t;
        CHECK(t.insert(5, 10));
        CHECK(!t.insert(5, 99));
        CHECK_EQ(t.size(), 1u);
        CHECK_EQ(*t.find(5), 10);
    }

    // Search: var olan ve olmayan anahtarlar
    {
        AVLTree<int, int> t;
        for (int i = 1; i <= 50; ++i) t.insert(i * 2, i * 100);  // çift sayılar
        CHECK(t.contains(2));
        CHECK(t.contains(100));
        CHECK_EQ(*t.find(40), 2000);
        CHECK(!t.contains(3));    // tek sayı yok
        CHECK(!t.contains(0));    // aralığın altı
        CHECK(!t.contains(102));  // aralığın üstü

        // const olmayan find ile değer değiştirme
        *t.find(40) = 7;
        CHECK_EQ(*t.find(40), 7);
    }

    // In-order gezinti: rastgele sırayla eklenenler artan çıkmalı
    {
        AVLTree<int, int> t;
        const int keys[] = {50, 20, 70, 10, 30, 60, 80, 25, 35, 5};
        for (int k : keys) t.insert(k, k);
        std::vector<int> out;
        t.inOrder([&out](int k, int) { out.push_back(k); });
        const std::vector<int> expected = {5, 10, 20, 25, 30, 35, 50, 60, 70, 80};
        CHECK(out == expected);
    }

    // Boş ağaçta gezinti hiçbir şey çağırmamalı
    {
        AVLTree<int, int> t;
        int calls = 0;
        t.inOrder([&calls](int, int) { ++calls; });
        CHECK_EQ(calls, 0);
    }

    // 1..1000 sıralı ekleme: sıradan BST'de yükseklik 1000 olurdu.
    // AVL'de en fazla ~1.44*log2(n) = ~14 olmalı.
    {
        AVLTree<int, int> t;
        for (int i = 1; i <= 1000; ++i) CHECK(t.insert(i, i));
        CHECK_EQ(t.size(), 1000u);
        CHECK(t.isBalanced());
        CHECK(t.height() <= 14);
        CHECK(t.height() >= 10);
        for (int i = 1; i <= 1000; ++i) CHECK(t.contains(i));
        CHECK(!t.contains(1001));
    }

    // Ters sıralı ekleme
    {
        AVLTree<int, int> t;
        for (int i = 1000; i >= 1; --i) t.insert(i, i);
        CHECK(t.isBalanced());
        CHECK(t.height() <= 14);
    }

    return finish("test_avl");
}