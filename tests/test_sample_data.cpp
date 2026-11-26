#include <cstdio>
#include <set>
#include <string>

#include "test_framework.h"
#include "managers/ClothingManager.h"
#include "storage/JsonManager.h"
#include "storage/SampleData.h"
#include "text/Normalize.h"

using namespace style;

int main() {
    // Sayı, geçerlilik, ardışık ID
    {
        ClothingCollection c = generateSampleClothes(60);
        CHECK_EQ(c.size(), 60u);
        bool allValid = true;
        int expectedId = 1001;
        for (const Clothing& x : c.all()) {
            if (!x.isValid()) allValid = false;
            if (x.id != expectedId++) allValid = false;
        }
        CHECK(allValid);
        CHECK_EQ(c.findById(1001)->imagePath, "images/1001.jpg");
    }

    // Aynı tohum aynı veri, farklı tohum farklı veri
    {
        ClothingCollection a = generateSampleClothes(100, 7);
        ClothingCollection b = generateSampleClothes(100, 7);
        ClothingCollection d = generateSampleClothes(100, 8);
        bool same = true, differs = false;
        for (std::size_t i = 0; i < a.size(); ++i) {
            if (a.all()[i].name != b.all()[i].name || a.all()[i].price != b.all()[i].price)
                same = false;
            if (a.all()[i].name != d.all()[i].name) differs = true;
        }
        CHECK(same);
        CHECK(differs);
    }

    // 60 üründe tüm kategoriler var ve fiyatlar mantıklı
    {
        ClothingCollection c = generateSampleClothes(60);
        std::set<Category> cats;
        bool pricesOk = true;
        for (const Clothing& x : c.all()) {
            cats.insert(x.category);
            if (x.price < 100 || x.price > 7000) pricesOk = false;
        }
        CHECK_EQ(cats.size(), 6u);
        CHECK(pricesOk);
    }

    // Türkçe karakterli ad üretilmeli ve Trie bunları bulmalı
    {
        ClothingCollection c = generateSampleClothes(60);
        bool hasNonAscii = false;
        for (const Clothing& x : c.all())
            for (unsigned char ch : x.name)
                if (ch >= 0x80) hasNonAscii = true;
        CHECK(hasNonAscii);

        ClothingManager m;
        CHECK_EQ(m.loadFrom(c), 60u);  // hepsi ClothingManager'a eklenebilmeli

        // Ad "renk + model" biçiminde; Türkçe harfli bir ürün adını
        // büyük harfli ve harfi bozulmuş yazımla da bulabilmeliyiz.
        const Clothing& first = c.all().front();
        CHECK(!m.suggestByName(first.name, 10).empty());
        CHECK(!m.suggestByName(normalize(first.name), 10).empty());
    }

    // Büyük üretim
    {
        ClothingCollection c = generateSampleClothes(5000);
        CHECK_EQ(c.size(), 5000u);
        ClothingManager m;
        CHECK_EQ(m.loadFrom(c), 5000u);
    }

    // JSON'a yaz, geri oku: Türkçe karakterler korunmalı
    {
        const std::string tmp = "tmp_test_sample.json";
        ClothingCollection src = generateSampleClothes(60);
        CHECK(JsonManager::saveClothes(tmp, src));
        ClothingCollection dst;
        LoadReport r = JsonManager::loadClothes(tmp, dst);
        CHECK(r.ok());
        CHECK_EQ(r.loaded, 60);
        CHECK(r.skipped.empty());
        bool same = true;
        for (std::size_t i = 0; i < src.size(); ++i)
            if (src.all()[i].name != dst.all()[i].name) same = false;
        CHECK(same);
        std::remove(tmp.c_str());
    }

    return finish("test_sample_data");
}