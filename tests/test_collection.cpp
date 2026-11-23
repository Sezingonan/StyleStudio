#include "test_framework.h"
#include "models/ClothingCollection.h"

using namespace style;

static Clothing make(int id, const char* name, double price) {
    Clothing c;
    c.id = id;
    c.name = name;
    c.price = price;
    return c;
}

int main() {
    ClothingCollection col;

    // Boş koleksiyon
    CHECK(col.empty());
    CHECK_EQ(col.size(), 0u);
    CHECK(col.findById(1) == nullptr);
    CHECK(!col.removeById(1));  // boşta silme çökmemeli

    // Ekleme
    CHECK(col.add(make(1, "Beyaz Tisort", 300)) == AddResult::Added);
    CHECK(col.add(make(2, "Siyah Pantolon", 900)) == AddResult::Added);
    CHECK_EQ(col.size(), 2u);

    // Tekrarlanan ID reddedilmeli, mevcut kayıt bozulmamalı
    CHECK(col.add(make(1, "Baska Urun", 50)) == AddResult::DuplicateId);
    CHECK_EQ(col.size(), 2u);
    CHECK_EQ(col.findById(1)->name, "Beyaz Tisort");

    // Geçersiz kayıtlar reddedilmeli
    CHECK(col.add(make(0, "IdSifir", 10)) == AddResult::InvalidItem);
    CHECK(col.add(make(3, "", 10)) == AddResult::InvalidItem);
    CHECK(col.add(make(4, "Eksi Fiyat", -1)) == AddResult::InvalidItem);
    CHECK_EQ(col.size(), 2u);

    // Arama
    CHECK(col.findById(2) != nullptr);
    CHECK_EQ(col.findById(2)->price, 900.0);
    CHECK(col.findById(99) == nullptr);

    // Silme
    CHECK(col.removeById(1));
    CHECK_EQ(col.size(), 1u);
    CHECK(col.findById(1) == nullptr);
    CHECK(!col.removeById(1));  // ikinci kez silinemez

    // Silinen ID tekrar kullanılabilir
    CHECK(col.add(make(1, "Yeni Tisort", 350)) == AddResult::Added);
    CHECK_EQ(col.size(), 2u);

    return finish("test_collection");
}