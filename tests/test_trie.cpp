#include "test_framework.h"
#include "structures/Trie.h"

using namespace style;

int main() {
    // Boş Trie
    {
        Trie t;
        CHECK(t.empty());
        CHECK(!t.startsWith("a"));
        CHECK(!t.startsWith(""));
        CHECK(t.find("a") == nullptr);
    }

    // Ekleme ve önek arama
    {
        Trie t;
        CHECK(t.insert("Pantolon", 1));
        CHECK(t.insert("Palto", 2));
        CHECK(t.insert("Tisort", 3));
        CHECK_EQ(t.size(), 3u);

        CHECK(t.startsWith("pan"));
        CHECK(t.startsWith("pa"));       // iki kelimenin ortak yolu
        CHECK(t.startsWith("Pantolon")); // tam kelime de önektir
        CHECK(t.startsWith(""));         // boş önek: Trie dolu
        CHECK(!t.startsWith("pak"));
        CHECK(!t.startsWith("pantolonx"));  // kelimeden uzun
        CHECK(!t.startsWith("z"));
    }

    // Büyük/küçük harf fark etmemeli
    {
        Trie t;
        t.insert("Pantolon", 1);
        CHECK(t.startsWith("PAN"));
        CHECK(t.startsWith("pAn"));
        CHECK(t.find("PANTOLON") != nullptr);
    }

    // Türkçe karakterler
    {
        Trie t;
        t.insert("Tişört", 7);
        CHECK(t.startsWith("tis"));
        CHECK(t.startsWith("tiş"));
        CHECK(t.startsWith("TİŞ"));
        CHECK(t.find("tisort") != nullptr);
        CHECK_EQ(t.find("TİŞÖRT")->front(), 7);
    }

    // Tam eşleşme: "pan" bir önek ama kayıtlı ad değil
    {
        Trie t;
        t.insert("Pantolon", 1);
        CHECK(t.startsWith("pan"));
        CHECK(t.find("pan") == nullptr);
        CHECK(t.find("pantolon") != nullptr);
    }

    // Aynı ismi taşıyan iki ürün: ikisinin ID'si de tutulmalı
    {
        Trie t;
        CHECK(t.insert("Beyaz Tisort", 10));
        CHECK(t.insert("Beyaz Tisort", 11));
        CHECK_EQ(t.size(), 2u);
        const std::vector<int>* ids = t.find("beyaz tisort");
        CHECK(ids != nullptr);
        if (ids) CHECK_EQ(ids->size(), 2u);
    }

    // Aynı (ad, id) çifti ikinci kez eklenemez
    {
        Trie t;
        CHECK(t.insert("Palto", 5));
        CHECK(!t.insert("Palto", 5));
        CHECK(!t.insert("PALTO", 5));  // normalize sonrası aynı
        CHECK_EQ(t.size(), 1u);
    }

    // Boş ad reddedilmeli
    {
        Trie t;
        CHECK(!t.insert("", 1));
        CHECK(!t.insert("\xF0\x9F\x98\x80", 2));  // normalize sonrası boş
        CHECK(t.empty());
    }

    // Boşluklu adlar
    {
        Trie t;
        t.insert("Siyah Pantolon", 1);
        CHECK(t.startsWith("siyah p"));
        CHECK(t.startsWith("siyah "));
        CHECK(!t.startsWith("siyahp"));
    }

    return finish("test_trie");
}