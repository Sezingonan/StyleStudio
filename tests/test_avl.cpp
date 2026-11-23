#include <vector>

#include "test_framework.h"
#include "structures/AVLTree.h"

using namespace style;

// Ağaçtaki anahtarları artan sırayla vektöre alır.
static std::vector<int> keysOf(const AVLTree<int, int>& t) {
    std::vector<int> out;
    t.inOrder([&out](int k, int) { out.push_back(k); });
    return out;
}

int main() {
    // Boş ağaç
    {
        AVLTree<int, int> t;
        CHECK(t.empty());
        CHECK_EQ(t.size(), 0u);
        CHECK_EQ(t.height(), 0);
        CHECK(t.isBalanced());
        CHECK(t.find(1) == nullptr);
        CHECK(!t.contains(1));
        CHECK(!t.erase(1));  // boş ağaçta silme çökmemeli
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
        for (int i = 1; i <= 50; ++i) t.insert(i * 2, i * 100);
        CHECK(t.contains(2));
        CHECK(t.contains(100));
        CHECK_EQ(*t.find(40), 2000);
        CHECK(!t.contains(3));
        CHECK(!t.contains(0));
        CHECK(!t.contains(102));
        *t.find(40) = 7;
        CHECK_EQ(*t.find(40), 7);
    }

    // In-order gezinti: rastgele sırayla eklenenler artan çıkmalı
    {
        AVLTree<int, int> t;
        const int keys[] = {50, 20, 70, 10, 30, 60, 80, 25, 35, 5};
        for (int k : keys) t.insert(k, k);
        const std::vector<int> expected = {5, 10, 20, 25, 30, 35, 50, 60, 70, 80};
        CHECK(keysOf(t) == expected);
    }

    // Boş ağaçta gezinti hiçbir şey çağırmamalı
    {
        AVLTree<int, int> t;
        int calls = 0;
        t.inOrder([&calls](int, int) { ++calls; });
        CHECK_EQ(calls, 0);
    }

    // Delete: yaprak (0 çocuk)
    {
        AVLTree<int, int> t;
        for (int k : {20, 10, 30}) t.insert(k, k);
        CHECK(t.erase(10));
        CHECK(!t.contains(10));
        CHECK_EQ(t.size(), 2u);
        CHECK((keysOf(t) == std::vector<int>{20, 30}));
        CHECK(t.isBalanced());
    }

    // Delete: tek çocuklu düğüm
    {
        AVLTree<int, int> t;
        for (int k : {20, 10, 30, 5}) t.insert(k, k);  // 10'un tek çocuğu 5
        CHECK(t.erase(10));
        CHECK((keysOf(t) == std::vector<int>{5, 20, 30}));
        CHECK(t.isBalanced());
    }

    // Delete: iki çocuklu düğüm (kök dahil)
    {
        AVLTree<int, int> t;
        for (int k : {50, 30, 70, 20, 40, 60, 80}) t.insert(k, k * 10);
        CHECK(t.erase(50));  // kök, iki çocuklu; successor = 60
        CHECK(!t.contains(50));
        CHECK_EQ(t.size(), 6u);
        CHECK((keysOf(t) == std::vector<int>{20, 30, 40, 60, 70, 80}));
        CHECK_EQ(*t.find(60), 600);  // değerler yerinde kalmış olmalı
        CHECK(t.isBalanced());
    }

    // Delete: olmayan anahtar ağacı değiştirmemeli
    {
        AVLTree<int, int> t;
        for (int k : {2, 1, 3}) t.insert(k, k);
        CHECK(!t.erase(99));
        CHECK_EQ(t.size(), 3u);
    }

    // Delete sonrası rotasyon gerekmeli: 1 silinince sağ taraf ağırlaşır
    {
        AVLTree<int, int> t;
        for (int k : {2, 1, 4, 3, 5}) t.insert(k, k);
        CHECK(t.isBalanced());
        CHECK(t.erase(1));  // 2'nin sağı ağır olur -> rotasyon
        CHECK(t.isBalanced());
        CHECK((keysOf(t) == std::vector<int>{2, 3, 4, 5}));
    }

    // Tek düğümü silince ağaç boşalmalı
    {
        AVLTree<int, int> t;
        t.insert(7, 7);
        CHECK(t.erase(7));
        CHECK(t.empty());
        CHECK_EQ(t.height(), 0);
        CHECK(!t.erase(7));  // ikinci kez silinemez
    }

    // Silinen anahtar tekrar eklenebilmeli
    {
        AVLTree<int, int> t;
        t.insert(1, 10);
        t.erase(1);
        CHECK(t.insert(1, 20));
        CHECK_EQ(*t.find(1), 20);
    }

    // 1..1000 sıralı ekleme: yükseklik ~log n kalmalı
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

    // Büyük silme testi: çift sayıları sil, ağaç her adımda dengeli kalmalı
    {
        AVLTree<int, int> t;
        for (int i = 1; i <= 1000; ++i) t.insert(i, i);
        bool alwaysBalanced = true;
        for (int i = 2; i <= 1000; i += 2) {
            if (!t.erase(i)) alwaysBalanced = false;
            if (!t.isBalanced()) alwaysBalanced = false;
        }
        CHECK(alwaysBalanced);
        CHECK_EQ(t.size(), 500u);
        for (int i = 1; i <= 1000; ++i) CHECK_EQ(t.contains(i), i % 2 == 1);
        CHECK(t.height() <= 13);

        // Kalanları da sil: ağaç tamamen boşalmalı
        for (int i = 1; i <= 1000; i += 2) t.erase(i);
        CHECK(t.empty());
        CHECK_EQ(t.height(), 0);
    }

    return finish("test_avl");
}