#include <vector>

#include "test_framework.h"
#include "managers/CatalogQuery.h"

using namespace style;
using Ids = std::vector<int>;

static Clothing make(int id, const char* name, Category cat, Color col, Style sty,
                     Season sea, double price) {
    Clothing c;
    c.id = id; c.name = name; c.category = cat; c.color = col;
    c.style = sty; c.season = sea; c.price = price;
    return c;
}

static Ids idsOf(const std::vector<Clothing>& v) {
    Ids out;
    for (const Clothing& c : v) out.push_back(c.id);
    return out;
}

int main() {
    ClothingManager m;
    m.add(make(1, "Beyaz Tisort", Category::Top, Color::White, Style::Casual, Season::All, 300));
    m.add(make(2, "Siyah Pantolon", Category::Bottom, Color::Black, Style::Casual, Season::All, 900));
    m.add(make(3, "Palazzo Pantolon", Category::Bottom, Color::Black, Style::Casual, Season::Summer, 1200));
    m.add(make(4, "Kahverengi Bot", Category::Shoes, Color::Brown, Style::Classic, Season::Winter, 2100));
    m.add(make(5, "Bej Pantolon", Category::Bottom, Color::Beige, Style::Classic, Season::Spring, 850));
    m.add(make(6, "Cizgili Tisort", Category::Top, Color::Blue, Style::Casual, Season::Summer, 300));

    // Boş yönetici
    {
        ClothingManager empty;
        CatalogFilter f;
        f.minPrice = 0;
        CHECK(queryCatalog(empty, f).empty());
        CHECK(queryCatalog(empty, CatalogFilter{}).empty());
    }

    // Filtresiz: hepsi, ID sırasıyla
    CHECK((idsOf(queryCatalog(m, CatalogFilter{})) == Ids{1, 2, 3, 4, 5, 6}));

    // Kategori, renk, stil
    {
        CatalogFilter f; f.category = Category::Bottom;
        CHECK((idsOf(queryCatalog(m, f)) == Ids{2, 3, 5}));
        CatalogFilter g; g.color = Color::Black;
        CHECK((idsOf(queryCatalog(m, g)) == Ids{2, 3}));
        CatalogFilter h; h.style = Style::Classic;
        CHECK((idsOf(queryCatalog(m, h)) == Ids{4, 5}));
    }

    // Mevsim: All olanlar her mevsimde çıkar
    {
        CatalogFilter f; f.season = Season::Summer;
        CHECK((idsOf(queryCatalog(m, f)) == Ids{1, 2, 3, 6}));
        CatalogFilter g; g.season = Season::Winter;
        CHECK((idsOf(queryCatalog(m, g)) == Ids{1, 2, 4}));
    }

    // Birleşik filtre
    {
        CatalogFilter f;
        f.category = Category::Bottom;
        f.season = Season::Summer;
        CHECK((idsOf(queryCatalog(m, f)) == Ids{2, 3}));
        f.color = Color::Beige;
        CHECK(queryCatalog(m, f).empty());
    }

    // Fiyat aralığı (sınırlar dahil), sıralamasız -> ID sırası
    {
        CatalogFilter f; f.minPrice = 300; f.maxPrice = 900;
        CHECK((idsOf(queryCatalog(m, f)) == Ids{1, 2, 5, 6}));
        CatalogFilter g; g.minPrice = 1000;
        CHECK((idsOf(queryCatalog(m, g)) == Ids{3, 4}));
        CatalogFilter h; h.maxPrice = 299;
        CHECK(queryCatalog(m, h).empty());
    }

    // Geçersiz aralık: min > max
    {
        CatalogFilter f; f.minPrice = 1000; f.maxPrice = 500;
        CHECK(queryCatalog(m, f).empty());
    }

    // Sıralamalar
    {
        CatalogFilter f; f.order = SortOrder::PriceAsc;
        auto asc = idsOf(queryCatalog(m, f));
        CHECK_EQ(asc.front() == 1 || asc.front() == 6, true);  // 300 TL'lik ikisi
        CHECK_EQ(asc.back(), 4);

        f.order = SortOrder::PriceDesc;
        auto desc = idsOf(queryCatalog(m, f));
        CHECK_EQ(desc.front(), 4);
        CHECK_EQ(desc[1], 3);

        f.order = SortOrder::NameAsc;
        // bej < beyaz < cizgili < kahverengi < palazzo < siyah
        CHECK((idsOf(queryCatalog(m, f)) == Ids{5, 1, 6, 4, 3, 2}));
    }

    // Filtre + sıralama birlikte
    {
        CatalogFilter f;
        f.category = Category::Bottom;
        f.order = SortOrder::PriceDesc;
        CHECK((idsOf(queryCatalog(m, f)) == Ids{3, 2, 5}));
        f.minPrice = 880;
        CHECK((idsOf(queryCatalog(m, f)) == Ids{3, 2}));
    }

    return finish("test_catalog");
}