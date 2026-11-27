#include "test_framework.h"
#include "managers/OutfitRules.h"

using namespace style;

static Clothing make(int id, Category cat) {
    Clothing c;
    c.id = id;
    c.name = "urun" + std::to_string(id);
    c.category = cat;
    c.price = 100;
    return c;
}

int main() {
    ClothingManager m;
    m.add(make(1, Category::Top));
    m.add(make(2, Category::Bottom));
    m.add(make(3, Category::Bottom));
    m.add(make(4, Category::Accessory));
    m.add(make(5, Category::Accessory));
    m.add(make(6, Category::Accessory));
    m.add(make(7, Category::Accessory));

    // Farklı kategoriler eklenir
    {
        OutfitEditor e;
        CHECK(placeInOutfit(e, m, 1) == PlaceResult::Added);
        CHECK(placeInOutfit(e, m, 2) == PlaceResult::Added);
        CHECK_EQ(e.size(), 2u);
    }

    // Aynı kategoride ikinci alt giyim ilkinin yerini alır
    {
        OutfitEditor e;
        placeInOutfit(e, m, 2);
        CHECK(placeInOutfit(e, m, 3) == PlaceResult::Replaced);
        CHECK_EQ(e.size(), 1u);
        CHECK(e.contains(3));
        CHECK(!e.contains(2));
    }

    // Aksesuar en fazla 3
    {
        OutfitEditor e;
        CHECK(placeInOutfit(e, m, 4) == PlaceResult::Added);
        CHECK(placeInOutfit(e, m, 5) == PlaceResult::Added);
        CHECK(placeInOutfit(e, m, 6) == PlaceResult::Added);
        CHECK(placeInOutfit(e, m, 7) == PlaceResult::Rejected);
        CHECK_EQ(e.size(), 3u);
    }

    // Aynı ürün tekrar, olmayan ürün
    {
        OutfitEditor e;
        placeInOutfit(e, m, 1);
        CHECK(placeInOutfit(e, m, 1) == PlaceResult::Rejected);
        CHECK(placeInOutfit(e, m, 999) == PlaceResult::Rejected);
        CHECK_EQ(e.size(), 1u);
    }

    // Değiştirme geri alınabilir
    {
        OutfitEditor e;
        placeInOutfit(e, m, 2);
        placeInOutfit(e, m, 3);   // 2 -> 3 (iki Undo adımı)
        CHECK(e.undo());
        CHECK(e.undo());
        CHECK(e.contains(2));
        CHECK(!e.contains(3));
    }

    return finish("test_outfit_rules");
}