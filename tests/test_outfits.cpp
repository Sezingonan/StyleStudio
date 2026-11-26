#include <cstdio>
#include <fstream>
#include <string>
#include <vector>

#include "test_framework.h"
#include "managers/OutfitManager.h"
#include "storage/OutfitStorage.h"

using namespace style;
using Ids = std::vector<int>;

static Clothing make(int id, double price) {
    Clothing c;
    c.id = id;
    c.name = "urun" + std::to_string(id);
    c.price = price;
    return c;
}

static void writeFile(const std::string& path, const std::string& text) {
    std::ofstream f(path);
    f << text;
}

int main() {
    const std::string tmp = "tmp_test_outfits.json";

    // ---------- Outfit.isValid ----------
    {
        Outfit o;
        CHECK(!o.isValid());
        o.outfitId = 1; o.name = "A"; o.clothingIds = {1, 2};
        CHECK(o.isValid());
        Outfit noName = o; noName.name = "";
        CHECK(!noName.isValid());
        Outfit empty = o; empty.clothingIds.clear();
        CHECK(!empty.isValid());
        Outfit dup = o; dup.clothingIds = {1, 1};
        CHECK(!dup.isValid());
        Outfit badItem = o; badItem.clothingIds = {1, 0};
        CHECK(!badItem.isValid());
        Outfit badId = o; badId.outfitId = 0;
        CHECK(!badId.isValid());
    }

    // ---------- OutfitManager ----------
    {
        OutfitManager m;
        CHECK(m.empty());
        CHECK(m.find(1) == nullptr);
        CHECK(!m.remove(1));
        CHECK(!m.update(1, "x", {1}));

        CHECK_EQ(m.create("Yaz", {1, 2}), 1);
        CHECK_EQ(m.create("Kis", {3}), 2);
        CHECK_EQ(m.size(), 2u);
        CHECK_EQ(m.create("", {1}), 0);        // isimsiz
        CHECK_EQ(m.create("Bos", {}), 0);      // urunsuz
        CHECK_EQ(m.create("Cift", {4, 4}), 0); // tekrarlı ürün
        CHECK_EQ(m.size(), 2u);                // reddedilenler kaydedilmedi

        // Silinen ID tekrar verilmez
        CHECK(m.remove(2));
        CHECK_EQ(m.create("Yeni", {5}), 3);

        // Güncelleme
        CHECK(m.update(1, "Yaz 2", {1, 2, 9}));
        CHECK_EQ(m.find(1)->name, "Yaz 2");
        CHECK((m.find(1)->clothingIds == Ids{1, 2, 9}));
        CHECK(!m.update(1, "", {1}));          // geçersiz güncelleme
        CHECK_EQ(m.find(1)->name, "Yaz 2");    // eski hâl korunmuş
        CHECK(!m.update(99, "x", {1}));
    }

    // addExisting ve sonraki ID
    {
        OutfitManager m;
        Outfit o; o.outfitId = 10; o.name = "Eski"; o.clothingIds = {1};
        CHECK(m.addExisting(o) == AddResult::Added);
        CHECK(m.addExisting(o) == AddResult::DuplicateId);
        Outfit bad; bad.outfitId = 11;
        CHECK(m.addExisting(bad) == AddResult::InvalidItem);
        CHECK_EQ(m.create("Yeni", {2}), 11);   // 10'dan sonra devam eder
    }

    // Toplam fiyat ve silinmiş ürün
    {
        ClothingManager items;
        items.add(make(1, 300));
        items.add(make(2, 900));
        Outfit o; o.outfitId = 1; o.name = "A"; o.clothingIds = {1, 2, 99};
        CHECK_EQ(OutfitManager::totalPrice(o, items), 1200.0);
        CHECK_EQ(OutfitManager::missingCount(o, items), 1u);
        items.remove(2);
        CHECK_EQ(OutfitManager::totalPrice(o, items), 300.0);
        CHECK_EQ(OutfitManager::missingCount(o, items), 2u);
    }

    // ---------- OutfitStorage ----------
    // Olmayan dosya
    {
        OutfitManager m;
        LoadReport r = OutfitStorage::load("yok_boyle_dosya.json", m);
        CHECK(!r.ok());
        CHECK(m.empty());
    }

    // Bozuk JSON ve dizi olmayan kök
    {
        OutfitManager m;
        writeFile(tmp, "{ bozuk");
        CHECK(!OutfitStorage::load(tmp, m).ok());
        writeFile(tmp, R"({"outfitId": 1})");
        CHECK(!OutfitStorage::load(tmp, m).ok());
        CHECK(m.empty());
    }

    // Boş dizi
    {
        OutfitManager m;
        writeFile(tmp, "[]");
        LoadReport r = OutfitStorage::load(tmp, m);
        CHECK(r.ok());
        CHECK_EQ(r.loaded, 0);
    }

    // Karışık dosya: 1 geçerli, 6 hatalı
    {
        writeFile(tmp, R"([
          {"outfitId":1,"name":"Iyi","clothingIds":[1,2]},
          {"outfitId":2,"clothingIds":[1]},
          {"outfitId":3,"name":"X","clothingIds":"yok"},
          {"outfitId":4,"name":"Y","clothingIds":[1,"a"]},
          {"outfitId":1,"name":"Tekrar","clothingIds":[5]},
          {"outfitId":6,"name":"Bos","clothingIds":[]},
          7
        ])");
        OutfitManager m;
        LoadReport r = OutfitStorage::load(tmp, m);
        CHECK(r.ok());
        CHECK_EQ(r.loaded, 1);
        CHECK_EQ(r.skipped.size(), 6u);
        CHECK_EQ(m.size(), 1u);
        CHECK_EQ(m.find(1)->name, "Iyi");
    }

    // Kaydet -> yükle gidiş-dönüşü (Türkçe karakterli ad dahil)
    {
        OutfitManager src;
        src.create("Yaz Kombini ş", {1001, 1002, 1004});
        src.create("Ofis", {1005});
        CHECK(OutfitStorage::save(tmp, src));

        OutfitManager dst;
        LoadReport r = OutfitStorage::load(tmp, dst);
        CHECK(r.ok());
        CHECK_EQ(r.loaded, 2);
        CHECK_EQ(dst.find(1)->name, "Yaz Kombini ş");
        CHECK((dst.find(1)->clothingIds == Ids{1001, 1002, 1004}));
        CHECK((dst.find(2)->clothingIds == Ids{1005}));
        CHECK_EQ(dst.create("Sonraki", {1}), 3);  // ID devam ediyor
    }

    // Yazılamayan yol
    {
        OutfitManager m;
        CHECK(!OutfitStorage::save("olmayan_klasor/x.json", m));
    }

    std::remove(tmp.c_str());
    return finish("test_outfits");
}