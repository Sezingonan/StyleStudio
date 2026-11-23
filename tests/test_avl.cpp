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

    // Tekrarlanan anahtar reddedilmeli
    {
        AVLTree<int, int> t;
        CHECK(t.insert(5, 10));
        CHECK(!t.insert(5, 99));
        CHECK_EQ(t.size(), 1u);
    }

    // 1..1000 sıralı ekleme: sıradan BST'de yükseklik 1000 olurdu.
    // AVL'de en fazla ~1.44*log2(n) = ~14 olmalı.
    {
        AVLTree<int, int> t;
        for (int i = 1; i <= 1000; ++i) CHECK(t.insert(i, i));
        CHECK_EQ(t.size(), 1000u);
        CHECK(t.isBalanced());
        CHECK(t.height() <= 14);
        CHECK(t.height() >= 10);  // log2(1000) ~ 9.97, alt sınır
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