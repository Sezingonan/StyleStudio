#include <vector>

#include "test_framework.h"
#include "structures/Trie.h"

using namespace style;

using Ids = std::vector<int>;

int main() {
    // Boş Trie
    {
        Trie t;
        CHECK(t.empty());
        CHECK(!t.startsWith("a"));
        CHECK(!t.startsWith(""));
        CHECK(t.find("a") == nullptr);
        CHECK(t.suggest("a", 5).empty());
        CHECK(t.suggest("", 5).empty());
        CHECK(!t.remove("a", 1));  // boşta silme çökmemeli
    }

    // Ekleme ve önek arama
    {
        Trie t;
        CHECK(t.insert("Pantolon", 1));
        CHECK(t.insert("Palto", 2));
        CHECK(t.insert("Tisort", 3));
        CHECK_EQ(t.size(), 3u);

        CHECK(t.startsWith("pan"));
        CHECK(t.startsWith("pa"));
        CHECK(t.startsWith("Pantolon"));
        CHECK(t.startsWith(""));
        CHECK(!t.startsWith("pak"));
        CHECK(!t.startsWith("pantolonx"));
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
        CHECK(!t.insert("PALTO", 5));
        CHECK_EQ(t.size(), 1u);
    }

    // Boş ad reddedilmeli
    {
        Trie t;
        CHECK(!t.insert("", 1));
        CHECK(!t.insert("\xF0\x9F\x98\x80", 2));
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

    // Suggest: alfabetik sıra ve önek filtresi
    {
        Trie t;
        t.insert("Pantolon", 1);
        t.insert("Palto", 2);
        t.insert("Panço", 3);   // normalize: "panco"
        t.insert("Tisort", 4);
        // palto < panco < pantolon
        CHECK((t.suggest("pa", 10) == Ids{2, 3, 1}));
        CHECK((t.suggest("pan", 10) == Ids{3, 1}));
        CHECK((t.suggest("PANT", 10) == Ids{1}));
        CHECK((t.suggest("t", 10) == Ids{4}));
    }

    // Suggest: k sınırı
    {
        Trie t;
        t.insert("Pantolon", 1);
        t.insert("Palto", 2);
        t.insert("Panço", 3);
        CHECK((t.suggest("pa", 2) == Ids{2, 3}));
        CHECK((t.suggest("pa", 1) == Ids{2}));
        CHECK(t.suggest("pa", 0).empty());
        CHECK_EQ(t.suggest("pa", 100).size(), 3u);
    }

    // Suggest: bulunmayan önek ve boş önek
    {
        Trie t;
        t.insert("Pantolon", 1);
        t.insert("Tisort", 2);
        CHECK(t.suggest("xyz", 5).empty());
        CHECK((t.suggest("", 5) == Ids{1, 2}));  // boş önek: hepsi, alfabetik
    }

    // Suggest: kelimenin kendisi de önek, kısa ad önce gelir
    {
        Trie t;
        t.insert("Pantolon", 1);
        t.insert("Pan", 4);
        CHECK((t.suggest("pan", 10) == Ids{4, 1}));
    }

    // Suggest: aynı adlı ürünler ekleme sırasıyla
    {
        Trie t;
        t.insert("Beyaz Tisort", 10);
        t.insert("Beyaz Tisort", 11);
        CHECK((t.suggest("beyaz", 10) == Ids{10, 11}));
        CHECK((t.suggest("beyaz", 1) == Ids{10}));
    }

    // Suggest: Türkçe
    {
        Trie t;
        t.insert("Tişört", 7);
        CHECK((t.suggest("TİŞ", 5) == Ids{7}));
        CHECK((t.suggest("tis", 5) == Ids{7}));
    }

    // Remove: temel
    {
        Trie t;
        t.insert("Pantolon", 1);
        CHECK(t.remove("Pantolon", 1));
        CHECK(t.empty());
        CHECK(t.find("pantolon") == nullptr);
        CHECK(!t.startsWith("pan"));  // yol budanmış olmalı
        CHECK(t.suggest("pan", 5).empty());
    }

    // Remove: olmayan ad, yanlış id, ikinci silme
    {
        Trie t;
        t.insert("Pantolon", 1);
        CHECK(!t.remove("Palto", 1));      // ad yok
        CHECK(!t.remove("Pantolon", 99));  // id yanlış
        CHECK(!t.remove("Pan", 1));        // sadece önek, kayıtlı ad değil
        CHECK_EQ(t.size(), 1u);
        CHECK(t.remove("PANTOLON", 1));    // normalize ile eşleşir
        CHECK(!t.remove("Pantolon", 1));   // ikinci kez silinemez
    }

    // Remove: ortak yol korunmalı
    {
        Trie t;
        t.insert("Pantolon", 1);
        t.insert("Palto", 2);
        CHECK(t.remove("Pantolon", 1));
        CHECK(t.startsWith("pa"));      // palto için gerekli
        CHECK(t.startsWith("palto"));
        CHECK(!t.startsWith("pan"));    // pantolon dalı gitmiş
        CHECK((t.suggest("pa", 5) == Ids{2}));
    }

    // Remove: kısa ad silinince uzun ad etkilenmemeli (ve tersi)
    {
        Trie t;
        t.insert("Pan", 4);
        t.insert("Pantolon", 1);
        CHECK(t.remove("Pan", 4));
        CHECK(t.find("pan") == nullptr);
        CHECK(t.find("pantolon") != nullptr);
        CHECK(t.startsWith("pantolon"));

        Trie u;
        u.insert("Pan", 4);
        u.insert("Pantolon", 1);
        CHECK(u.remove("Pantolon", 1));
        CHECK(u.find("pan") != nullptr);
        CHECK(!u.startsWith("pant"));
    }

    // Remove: aynı adlı iki üründen yalnızca biri silinir
    {
        Trie t;
        t.insert("Beyaz Tisort", 10);
        t.insert("Beyaz Tisort", 11);
        CHECK(t.remove("Beyaz Tisort", 10));
        CHECK_EQ(t.size(), 1u);
        CHECK((t.suggest("beyaz", 5) == Ids{11}));
        CHECK(t.remove("Beyaz Tisort", 11));
        CHECK(t.empty());
        CHECK(!t.startsWith("b"));
    }

    // Silinen ad tekrar eklenebilmeli
    {
        Trie t;
        t.insert("Palto", 2);
        t.remove("Palto", 2);
        CHECK(t.insert("Palto", 2));
        CHECK((t.suggest("pal", 5) == Ids{2}));
    }

    // Büyük test: 1000 ad ekle, hepsini sil, Trie tamamen boşalmalı
    {
        Trie t;
        for (int i = 1; i <= 1000; ++i) t.insert("urun" + std::to_string(i), i);
        CHECK_EQ(t.size(), 1000u);
        CHECK_EQ(t.suggest("urun", 5000).size(), 1000u);
        CHECK_EQ(t.suggest("urun12", 5000).size(), 11u);  // 12, 120-129

        bool allRemoved = true;
        for (int i = 1; i <= 1000; ++i) {
            if (!t.remove("urun" + std::to_string(i), i)) allRemoved = false;
        }
        CHECK(allRemoved);
        CHECK(t.empty());
        CHECK(!t.startsWith("u"));  // tüm dallar budanmış
    }

    return finish("test_trie");
}