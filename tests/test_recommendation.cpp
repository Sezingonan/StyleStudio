#include <algorithm>
#include <vector>

#include "test_framework.h"
#include "managers/RecommendationManager.h"

using namespace style;

static Clothing make(int id, Category cat, Color col, Style sty) {
    Clothing c;
    c.id = id;
    c.name = "urun" + std::to_string(id);
    c.category = cat;
    c.color = col;
    c.style = sty;
    c.price = 100;
    return c;
}

static void fill(ClothingManager& m) {
    m.add(make(1, Category::Top, Color::White, Style::Casual));
    m.add(make(2, Category::Bottom, Color::Black, Style::Casual));
    m.add(make(3, Category::Bottom, Color::Black, Style::Casual));
    m.add(make(4, Category::Shoes, Color::Brown, Style::Classic));
    m.add(make(5, Category::Bottom, Color::Beige, Style::Classic));
    m.add(make(6, Category::Top, Color::Red, Style::Casual));
    m.add(make(7, Category::Bottom, Color::Blue, Style::Casual));
    m.add(make(8, Category::Shoes, Color::Black, Style::Casual));
}

static std::vector<int> sortedIds(const std::vector<RecommendationManager::Suggestion>& v) {
    std::vector<int> out;
    for (const auto& s : v) out.push_back(s.item.id);
    std::sort(out.begin(), out.end());
    return out;
}

static int levelOf(const std::vector<RecommendationManager::Suggestion>& v, int id) {
    for (const auto& s : v) if (s.item.id == id) return s.level;
    return -1;
}

using Ids = std::vector<int>;

int main() {
    // Boş yönetici
    {
        ClothingManager m;
        RecommendationManager r(m);
        r.rebuild();
        CHECK_EQ(r.graph().nodeCount(), 0u);
        CHECK(r.recommend(1).empty());
        CHECK(!r.onAdded(1));
        CHECK(!r.onRemoved(1));
    }

    // rebuild: düğüm ve kenar sayısı
    ClothingManager m;
    fill(m);
    RecommendationManager r(m);
    r.rebuild();
    CHECK_EQ(r.graph().nodeCount(), 8u);
    CHECK_EQ(r.graph().edgeCount(), 11u);
    CHECK(r.graph().hasEdge(1, 2));
    CHECK(!r.graph().hasEdge(2, 3));   // aynı kategori
    CHECK(!r.graph().hasEdge(6, 7));   // kırmızı-mavi uyumsuz
    CHECK(!r.graph().hasEdge(1, 4));   // stil grubu farklı
    CHECK(r.graph().hasEdge(4, 5));    // kahverengi-bej, Classic

    // Seviye 1: doğrudan uyumlular
    {
        auto s = r.recommend(1, 1);
        CHECK((sortedIds(s) == Ids{2, 3, 7, 8}));
        for (const auto& x : s) CHECK_EQ(x.level, 1);
    }

    // Seviye 2: tek dolaylı aday (6) aynı kategori olduğu için elenir
    {
        auto s = r.recommend(1, 2);
        CHECK((sortedIds(s) == Ids{2, 3, 7, 8}));
    }

    // Seviye 2'de gerçek dolaylı öneri: 6 -> (2,3,8) -> 7
    {
        auto s = r.recommend(6, 2);
        CHECK((sortedIds(s) == Ids{2, 3, 7, 8}));
        CHECK_EQ(levelOf(s, 2), 1);
        CHECK_EQ(levelOf(s, 7), 2);
    }

    // Ayrı bileşen
    {
        auto s = r.recommend(4, 3);
        CHECK((sortedIds(s) == Ids{5}));
    }

    // Sınırlar ve geçersiz girdiler
    CHECK_EQ(r.recommend(1, 1, 2).size(), 2u);
    CHECK(r.recommend(1, 1, 0).empty());
    CHECK(r.recommend(1, 0).empty());
    CHECK(r.recommend(999).empty());

    // Kural değişmeden: öneriler sıralı ve başlangıcı içermez
    {
        auto s = r.recommend(2, 3);
        CHECK(levelOf(s, 2) == -1);
    }

    // Artımlı ekleme, rebuild ile aynı grafı üretmeli
    {
        ClothingManager m2;
        RecommendationManager r2(m2);
        for (int id = 1; id <= 8; ++id) {
            Clothing c = *m.findById(id);
            CHECK(m2.add(c) == AddResult::Added);
            CHECK(r2.onAdded(id));
        }
        CHECK_EQ(r2.graph().nodeCount(), 8u);
        CHECK_EQ(r2.graph().edgeCount(), 11u);
        CHECK(!r2.onAdded(3));    // zaten var
        CHECK(!r2.onAdded(777));  // manager'da yok
    }

    // Silme: kenarlar temizlenmeli
    {
        CHECK(m.remove(8));
        CHECK(r.onRemoved(8));
        CHECK(!r.onRemoved(8));
        CHECK_EQ(r.graph().nodeCount(), 7u);
        CHECK_EQ(r.graph().edgeCount(), 6u);  // 8'in 5 kenarı gitti
        CHECK((sortedIds(r.recommend(1, 1)) == Ids{2, 3, 7}));
    }

    return finish("test_recommendation");
}