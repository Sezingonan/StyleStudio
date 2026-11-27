#include <string>
#include <vector>

#include "test_framework.h"
#include "managers/ClothingManager.h"

using namespace style;

static Clothing make(int id, const char* name, double price = 100.0) {
    Clothing c;
    c.id = id;
    c.name = name;
    c.price = price;
    return c;
}

static std::vector<int> idsOf(const std::vector<Clothing>& v) {
    std::vector<int> out;
    for (const Clothing& c : v) out.push_back(c.id);
    return out;
}

using Ids = std::vector<int>;

int main() {
    // Boş yönetici
    {
        ClothingManager m;
        CHECK(m.empty());
        CHECK(m.findById(1) == nullptr);
        CHECK(!m.remove(1));
        CHECK(m.suggestByName("a", 5).empty());
        CHECK(m.all().empty());
    }

    // Ekleme: AVL ve Trie birlikte güncellenmeli
    {
        ClothingManager m;
        CHECK(m.add(make(1, "Pantolon")) == AddResult::Added);
        CHECK(m.add(make(2, "Palto")) == AddResult::Added);
        CHECK_EQ(m.size(), 2u);
        CHECK(m.findById(1) != nullptr);
        CHECK_EQ(m.findById(1)->name, "Pantolon");
        CHECK((idsOf(m.suggestByName("pa", 10)) == Ids{2, 1}));  // palto < pantolon
    }

    // Reddedilen kayıtlar hiçbir yapıyı değiştirmemeli
    {
        ClothingManager m;
        m.add(make(1, "Pantolon"));
        CHECK(m.add(make(1, "Baska Urun")) == AddResult::DuplicateId);
        CHECK(m.add(make(0, "IdSifir")) == AddResult::InvalidItem);
        CHECK(m.add(make(3, "")) == AddResult::InvalidItem);
        CHECK(m.add(make(4, "Eksi", -1)) == AddResult::InvalidItem);
        CHECK(m.add(make(5, "\xF0\x9F\x98\x80")) == AddResult::InvalidItem);  // sadece emoji
        CHECK_EQ(m.size(), 1u);
        CHECK(m.findById(5) == nullptr);        // AVL'ye girmemiş
        CHECK(m.suggestByName("baska", 5).empty());  // Trie'ye girmemiş
        CHECK_EQ(m.findById(1)->name, "Pantolon");
    }

    // Silme: iki yapıdan da çıkmalı
    {
        ClothingManager m;
        m.add(make(1, "Pantolon"));
        m.add(make(2, "Palto"));
        CHECK(m.remove(1));
        CHECK(m.findById(1) == nullptr);
        CHECK_EQ(m.size(), 1u);
        CHECK((idsOf(m.suggestByName("pa", 10)) == Ids{2}));
        CHECK(m.suggestByName("pan", 10).empty());
        CHECK(!m.remove(1));  // ikinci kez silinemez
    }

    // Silinen ID ve ad tekrar eklenebilmeli
    {
        ClothingManager m;
        m.add(make(1, "Palto"));
        m.remove(1);
        CHECK(m.add(make(1, "Palto")) == AddResult::Added);
        CHECK_EQ(m.suggestByName("pal", 5).size(), 1u);
    }

    // Aynı adlı iki ürün: biri silinince diğeri kalmalı
    {
        ClothingManager m;
        m.add(make(10, "Beyaz Tisort"));
        m.add(make(11, "Beyaz Tisort"));
        CHECK((idsOf(m.suggestByName("beyaz", 5)) == Ids{10, 11}));
        CHECK(m.remove(10));
        CHECK((idsOf(m.suggestByName("beyaz", 5)) == Ids{11}));
    }

    // Türkçe ad ile arama
    {
        ClothingManager m;
        m.add(make(7, "Tişört"));
        CHECK((idsOf(m.suggestByName("tis", 5)) == Ids{7}));
        CHECK((idsOf(m.suggestByName("TİŞ", 5)) == Ids{7}));
        CHECK_EQ(m.suggestByName("tis", 5).front().name, "Tişört");  // asıl ad korunur
    }

    // all(): ID sırasıyla gelmeli
    {
        ClothingManager m;
        m.add(make(30, "C"));
        m.add(make(10, "A"));
        m.add(make(20, "B"));
        CHECK((idsOf(m.all()) == Ids{10, 20, 30}));
    }

    // loadFrom: koleksiyondan aktarma
    {
        ClothingCollection col;
        col.add(make(1, "Pantolon"));
        col.add(make(2, "Palto"));
        col.add(make(3, "Tisort"));
        ClothingManager m;
        CHECK_EQ(m.loadFrom(col), 3u);
        CHECK_EQ(m.size(), 3u);
        CHECK((idsOf(m.suggestByName("pa", 10)) == Ids{2, 1}));
        // ikinci kez yüklenirse hepsi tekrarlı olur, hiçbiri eklenmez
        CHECK_EQ(m.loadFrom(col), 0u);
        CHECK_EQ(m.size(), 3u);
    }

    // Kelime başı araması: adın herhangi bir kelimesiyle bulunabilmeli
    {
        ClothingManager m;
        m.add(make(1, "Siyah Pantolon"));
        m.add(make(2, "Gri Palazzo Pantolon"));
        m.add(make(3, "Beyaz Tişört"));
        CHECK((idsOf(m.suggestByName("pantolon", 10)) == Ids{1, 2}));
        CHECK((idsOf(m.suggestByName("pa", 10)) == Ids{2, 1}));   // 2 iki yoldan gelir, tek sayılır
        CHECK((idsOf(m.suggestByName("pa", 1)) == Ids{2}));
        CHECK((idsOf(m.suggestByName("tis", 10)) == Ids{3}));
        CHECK((idsOf(m.suggestByName("siyah pan", 10)) == Ids{1}));

        // Silinen ürünün kelime anahtarları da temizlenmeli
        CHECK(m.remove(1));
        CHECK((idsOf(m.suggestByName("pantolon", 10)) == Ids{2}));
        CHECK(m.suggestByName("siyah", 10).empty());
    }

    // Tekrarlayan kelime: aynı ürün bir kez dönmeli, k kadar benzersiz ürün bulunabilmeli
    {
        ClothingManager m;
        m.add(make(2, "Gri Palazzo Pantolon"));
        m.add(make(4, "Gri Gri Pantolon"));
        CHECK((idsOf(m.suggestByName("gri", 3)) == Ids{4, 2}));
        CHECK((idsOf(m.suggestByName("gri", 1)) == Ids{4}));
    }

    // Büyük test: 1000 ürün ekle, çiftleri sil, tutarlılık korunmalı
    {
        ClothingManager m;
        for (int i = 1; i <= 1000; ++i) {
            m.add(make(i, ("urun" + std::to_string(i)).c_str()));
        }
        CHECK_EQ(m.size(), 1000u);
        for (int i = 2; i <= 1000; i += 2) CHECK(m.remove(i));
        CHECK_EQ(m.size(), 500u);
        bool consistent = true;
        for (int i = 1; i <= 1000; ++i) {
            const bool inStore = m.findById(i) != nullptr;
            const bool inTrie =
                !m.suggestByName("urun" + std::to_string(i), 2000).empty() &&
                [&]() {
                    for (const Clothing& c : m.suggestByName("urun" + std::to_string(i), 2000))
                        if (c.id == i) return true;
                    return false;
                }();
            if (inStore != (i % 2 == 1)) consistent = false;
            if (inTrie != inStore) consistent = false;
        }
        CHECK(consistent);
        CHECK_EQ(m.suggestByName("urun", 5000).size(), 500u);
    }

    return finish("test_manager");
}