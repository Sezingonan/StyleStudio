#include "storage/SampleData.h"

#include <set>
#include <string>

namespace style {
namespace {

struct Base {
    const char* name;
    Category category;
    Style style1;
    Style style2;
    Season season;
    int minPrice;
    int maxPrice;
};

const Base kBases[] = {
    // Top
    {"Tişört", Category::Top, Style::Casual, Style::Street, Season::All, 250, 600},
    {"Gömlek", Category::Top, Style::Formal, Style::Classic, Season::All, 500, 1200},
    {"Sweatshirt", Category::Top, Style::Casual, Style::Street, Season::Autumn, 600, 1400},
    {"Kazak", Category::Top, Style::Classic, Style::Casual, Season::Winter, 700, 1800},
    {"Bluz", Category::Top, Style::Formal, Style::Classic, Season::Spring, 450, 1100},
    {"Spor Tişört", Category::Top, Style::Sport, Style::Sport, Season::Summer, 300, 700},
    {"Polo Yaka Tişört", Category::Top, Style::Casual, Style::Classic, Season::Summer, 400, 900},
    // Bottom
    {"Pantolon", Category::Bottom, Style::Casual, Style::Classic, Season::All, 600, 1500},
    {"Kot Pantolon", Category::Bottom, Style::Casual, Style::Street, Season::All, 700, 1600},
    {"Şort", Category::Bottom, Style::Casual, Style::Sport, Season::Summer, 300, 800},
    {"Etek", Category::Bottom, Style::Formal, Style::Classic, Season::Spring, 500, 1300},
    {"Eşofman Altı", Category::Bottom, Style::Sport, Style::Sport, Season::Autumn, 400, 900},
    {"Kumaş Pantolon", Category::Bottom, Style::Formal, Style::Classic, Season::All, 800, 2000},
    {"Palazzo Pantolon", Category::Bottom, Style::Casual, Style::Street, Season::Summer, 600, 1400},
    // Outerwear
    {"Ceket", Category::Outerwear, Style::Classic, Style::Formal, Season::Spring, 1200, 3500},
    {"Mont", Category::Outerwear, Style::Casual, Style::Street, Season::Winter, 1800, 5000},
    {"Palto", Category::Outerwear, Style::Formal, Style::Classic, Season::Winter, 2500, 6500},
    {"Hırka", Category::Outerwear, Style::Casual, Style::Classic, Season::Autumn, 700, 1800},
    {"Trençkot", Category::Outerwear, Style::Classic, Style::Formal, Season::Spring, 2000, 4500},
    {"Deri Ceket", Category::Outerwear, Style::Street, Style::Casual, Season::Autumn, 3000, 7000},
    // Shoes
    {"Sneaker", Category::Shoes, Style::Casual, Style::Street, Season::All, 1200, 3500},
    {"Bot", Category::Shoes, Style::Classic, Style::Casual, Season::Winter, 1500, 4000},
    {"Loafer", Category::Shoes, Style::Formal, Style::Classic, Season::All, 1400, 3200},
    {"Topuklu Ayakkabı", Category::Shoes, Style::Formal, Style::Classic, Season::Spring, 1300, 3000},
    {"Spor Ayakkabı", Category::Shoes, Style::Sport, Style::Sport, Season::All, 1500, 4000},
    {"Sandalet", Category::Shoes, Style::Casual, Style::Street, Season::Summer, 500, 1500},
    // Bag
    {"Sırt Çantası", Category::Bag, Style::Casual, Style::Sport, Season::All, 600, 2000},
    {"Omuz Çantası", Category::Bag, Style::Classic, Style::Formal, Season::All, 900, 3000},
    {"El Çantası", Category::Bag, Style::Formal, Style::Classic, Season::All, 1000, 3500},
    {"Çapraz Çanta", Category::Bag, Style::Casual, Style::Street, Season::All, 500, 1800},
    {"Spor Çanta", Category::Bag, Style::Sport, Style::Sport, Season::All, 500, 1500},
    // Accessory
    {"Kemer", Category::Accessory, Style::Classic, Style::Formal, Season::All, 250, 900},
    {"Şapka", Category::Accessory, Style::Casual, Style::Street, Season::Summer, 200, 700},
    {"Eşarp", Category::Accessory, Style::Classic, Style::Classic, Season::Winter, 300, 1000},
    {"Güneş Gözlüğü", Category::Accessory, Style::Casual, Style::Street, Season::Summer, 400, 2000},
    {"Saat", Category::Accessory, Style::Classic, Style::Formal, Season::All, 800, 5000},
    {"Atkı", Category::Accessory, Style::Casual, Style::Casual, Season::Winter, 250, 800},
    {"Bere", Category::Accessory, Style::Casual, Style::Street, Season::Winter, 150, 500},
};
const std::size_t kBaseCount = sizeof(kBases) / sizeof(kBases[0]);

struct ColorName {
    Color color;
    const char* name;
};

// Nötr renkler iki kez yazıldı: seçilme şansları iki kat.
const ColorName kPalette[] = {
    {Color::White, "Beyaz"},  {Color::White, "Beyaz"},
    {Color::Black, "Siyah"},  {Color::Black, "Siyah"},
    {Color::Gray, "Gri"},     {Color::Gray, "Gri"},
    {Color::Beige, "Bej"},    {Color::Beige, "Bej"},
    {Color::Brown, "Kahverengi"}, {Color::Blue, "Mavi"},
    {Color::Navy, "Lacivert"}, {Color::Red, "Kırmızı"},
    {Color::Green, "Yeşil"},  {Color::Pink, "Pembe"},
    {Color::Yellow, "Sarı"},
};
const std::size_t kPaletteCount = sizeof(kPalette) / sizeof(kPalette[0]);

// Basit, taşınabilir sözde rastgele üretici (standart kütüphane dağılımları
// platforma göre farklı sonuç verebilir; biz aynı çıktıyı istiyoruz).
struct Lcg {
    unsigned state;
    unsigned next() {
        state = state * 1664525u + 1013904223u;
        return state >> 8;
    }
    unsigned pick(unsigned n) { return next() % n; }
};

}  // namespace

ClothingCollection generateSampleClothes(std::size_t count, unsigned seed,
                                         int firstId) {
    ClothingCollection result;
    Lcg rng{seed};
    std::set<std::string> usedNames;

    for (std::size_t i = 0; i < count; ++i) {
        const Base& base = kBases[i % kBaseCount];

        // Aynı ad tekrar etmesin diye en fazla 20 kez farklı renk dene.
        const ColorName* chosen = &kPalette[rng.pick(static_cast<unsigned>(kPaletteCount))];
        std::string name = std::string(chosen->name) + " " + base.name;
        for (int attempt = 0; attempt < 20 && usedNames.count(name) > 0; ++attempt) {
            chosen = &kPalette[rng.pick(static_cast<unsigned>(kPaletteCount))];
            name = std::string(chosen->name) + " " + base.name;
        }
        usedNames.insert(name);

        Clothing c;
        c.id = firstId + static_cast<int>(i);
        c.name = name;
        c.category = base.category;
        c.color = chosen->color;
        c.style = rng.pick(2) == 0 ? base.style1 : base.style2;
        c.season = base.season;
        const int span = base.maxPrice - base.minPrice + 1;
        const int price = base.minPrice + static_cast<int>(rng.pick(static_cast<unsigned>(span)));
        c.price = static_cast<double>((price / 10) * 10);
        if (c.price < base.minPrice) c.price = base.minPrice;
        c.imagePath = "images/" + std::to_string(c.id) + ".jpg";
        result.add(c);
    }
    return result;
}

}  // namespace style