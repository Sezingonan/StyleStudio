#include "test_framework.h"
#include "managers/Compatibility.h"

using namespace style;

static Clothing make(int id, Category cat, Color col, Style sty) {
    Clothing c;
    c.id = id;
    c.name = "x";
    c.category = cat;
    c.color = col;
    c.style = sty;
    return c;
}

int main() {
    // Aynı ton
    CHECK(colorsCompatible(Color::Red, Color::Pink));
    CHECK(colorsCompatible(Color::Blue, Color::Navy));
    CHECK(colorsCompatible(Color::Beige, Color::Brown));
    CHECK(colorsCompatible(Color::Red, Color::Red));

    // Yan yana
    CHECK(colorsCompatible(Color::Yellow, Color::Green));
    CHECK(colorsCompatible(Color::Blue, Color::Green));
    CHECK(colorsCompatible(Color::Red, Color::Beige));
    CHECK(colorsCompatible(Color::Yellow, Color::Brown));

    // Karşılıklı
    CHECK(colorsCompatible(Color::Red, Color::Green));
    CHECK(colorsCompatible(Color::Pink, Color::Green));
    CHECK(colorsCompatible(Color::Brown, Color::Navy));
    CHECK(colorsCompatible(Color::Beige, Color::Blue));

    // Uyumsuz
    CHECK(!colorsCompatible(Color::Red, Color::Blue));
    CHECK(!colorsCompatible(Color::Yellow, Color::Red));
    CHECK(!colorsCompatible(Color::Yellow, Color::Blue));
    CHECK(!colorsCompatible(Color::Brown, Color::Green));
    CHECK(!colorsCompatible(Color::Pink, Color::Navy));

    // Simetri: sıra fark etmemeli (tüm renk çiftleri)
    {
        const Color all[] = {Color::White, Color::Black, Color::Gray, Color::Beige,
                             Color::Brown, Color::Blue, Color::Navy, Color::Red,
                             Color::Green, Color::Pink, Color::Yellow, Color::Other};
        bool symmetric = true;
        for (Color a : all)
            for (Color b : all)
                if (colorsCompatible(a, b) != colorsCompatible(b, a)) symmetric = false;
        CHECK(symmetric);
    }

    // Nötr renkler her şeyle uyumlu
    {
        const Color neutrals[] = {Color::White, Color::Black, Color::Gray, Color::Other};
        const Color all[] = {Color::White, Color::Black, Color::Gray, Color::Beige,
                             Color::Brown, Color::Blue, Color::Navy, Color::Red,
                             Color::Green, Color::Pink, Color::Yellow, Color::Other};
        bool ok = true;
        for (Color n : neutrals)
            for (Color c : all)
                if (!colorsCompatible(n, c)) ok = false;
        CHECK(ok);
    }

    // Stil
    CHECK(stylesCompatible(Style::Casual, Style::Street));
    CHECK(stylesCompatible(Style::Casual, Style::Sport));
    CHECK(stylesCompatible(Style::Street, Style::Sport));
    CHECK(stylesCompatible(Style::Formal, Style::Classic));
    CHECK(stylesCompatible(Style::Formal, Style::Formal));
    CHECK(!stylesCompatible(Style::Formal, Style::Sport));
    CHECK(!stylesCompatible(Style::Classic, Style::Casual));
    CHECK(!stylesCompatible(Style::Street, Style::Formal));

    // Kategori
    CHECK(categoriesCompatible(Category::Top, Category::Bottom));
    CHECK(!categoriesCompatible(Category::Bottom, Category::Bottom));

    // Birleşik kural
    {
        Clothing whiteTop = make(1, Category::Top, Color::White, Style::Casual);
        Clothing blackPants = make(2, Category::Bottom, Color::Black, Style::Casual);
        Clothing otherPants = make(3, Category::Bottom, Color::Blue, Style::Casual);
        Clothing formalShoes = make(4, Category::Shoes, Color::Black, Style::Formal);
        Clothing redTop = make(5, Category::Top, Color::Red, Style::Casual);
        Clothing bluePants = make(6, Category::Bottom, Color::Blue, Style::Casual);

        CHECK(areCompatible(whiteTop, blackPants));
        CHECK(areCompatible(blackPants, whiteTop));       // simetrik
        CHECK(areCompatible(whiteTop, otherPants));       // beyaz nötr
        CHECK(!areCompatible(whiteTop, formalShoes));     // stil uyumsuz
        CHECK(!areCompatible(blackPants, otherPants));    // aynı kategori
        CHECK(!areCompatible(redTop, bluePants));         // renk uyumsuz
        CHECK(!areCompatible(whiteTop, whiteTop));        // kendisiyle değil
    }

    return finish("test_compatibility");
}