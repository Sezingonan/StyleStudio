#include "test_framework.h"
#include "models/Enums.h"

using namespace style;

int main() {
    // 1) Enum -> yazı
    CHECK_EQ(toString(Category::Top), "Top");
    CHECK_EQ(toString(Color::Navy), "Navy");
    CHECK_EQ(toString(Style::Street), "Street");
    CHECK_EQ(toString(Season::All), "All");

    // 2) Yazı -> enum (doğru yazım)
    CHECK(parseColor("Black").has_value());
    CHECK(*parseColor("Black") == Color::Black);
    CHECK(*parseCategory("Shoes") == Category::Shoes);

    // 3) Büyük/küçük harf fark etmemeli
    CHECK(*parseColor("black") == Color::Black);
    CHECK(*parseColor("BLACK") == Color::Black);
    CHECK(*parseSeason("wInTeR") == Season::Winter);

    // 4) Hatalı yazı -> nullopt (çökme yok)
    CHECK(!parseColor("Blck").has_value());
    CHECK(!parseCategory("").has_value());
    CHECK(!parseStyle("nonexistent").has_value());
    CHECK(!parseSeason("   ").has_value());

    // 5) Gidiş-dönüş: her değer toString -> parse ile aynı değere dönmeli
    const Category cats[] = {Category::Top, Category::Bottom, Category::Outerwear,
                             Category::Shoes, Category::Bag, Category::Accessory};
    for (Category c : cats) {
        auto back = parseCategory(toString(c));
        CHECK(back.has_value() && *back == c);
    }

    const Color colors[] = {Color::White, Color::Black, Color::Gray, Color::Beige,
                            Color::Brown, Color::Blue, Color::Navy, Color::Red,
                            Color::Green, Color::Pink, Color::Yellow, Color::Other};
    for (Color c : colors) {
        auto back = parseColor(toString(c));
        CHECK(back.has_value() && *back == c);
    }

    const Style styles[] = {Style::Casual, Style::Formal, Style::Sport,
                            Style::Street, Style::Classic};
    for (Style s : styles) {
        auto back = parseStyle(toString(s));
        CHECK(back.has_value() && *back == s);
    }

    const Season seasons[] = {Season::Spring, Season::Summer, Season::Autumn,
                              Season::Winter, Season::All};
    for (Season s : seasons) {
        auto back = parseSeason(toString(s));
        CHECK(back.has_value() && *back == s);
    }

    return finish("test_enums");
}