#include <cstdio>
#include <fstream>
#include <string>

#include "test_framework.h"
#include "storage/JsonManager.h"

using namespace style;

static void writeFile(const std::string& path, const std::string& text) {
    std::ofstream f(path);
    f << text;
}

int main() {
    const std::string tmp = "tmp_test_clothes.json";

    // 1) Olmayan dosya: fatalError dolu, çökme yok
    {
        ClothingCollection col;
        LoadReport r = JsonManager::loadClothes("yok_boyle_dosya.json", col);
        CHECK(!r.ok());
        CHECK(col.empty());
    }

    // 2) Bozuk JSON
    {
        writeFile(tmp, "{ bozuk json");
        ClothingCollection col;
        LoadReport r = JsonManager::loadClothes(tmp, col);
        CHECK(!r.ok());
        CHECK(col.empty());
    }

    // 3) Kök eleman dizi değil
    {
        writeFile(tmp, R"({"id": 1})");
        ClothingCollection col;
        LoadReport r = JsonManager::loadClothes(tmp, col);
        CHECK(!r.ok());
    }

    // 4) Boş dizi: hata değil, 0 kayıt
    {
        writeFile(tmp, "[]");
        ClothingCollection col;
        LoadReport r = JsonManager::loadClothes(tmp, col);
        CHECK(r.ok());
        CHECK_EQ(r.loaded, 0);
    }

    // 5) Karışık dosya: 1 geçerli + hatalı kayıtlar
    {
        writeFile(tmp, R"([
          {"id":1,"name":"Iyi","category":"Top","color":"White","style":"Casual","season":"All","price":100},
          {"id":2,"name":"EksikAlan","category":"Top","color":"White","style":"Casual","season":"All"},
          {"id":3,"name":"YanlisRenk","category":"Top","color":"Blck","style":"Casual","season":"All","price":100},
          {"id":1,"name":"Tekrar","category":"Top","color":"White","style":"Casual","season":"All","price":100},
          {"id":5,"name":"EksiFiyat","category":"Top","color":"White","style":"Casual","season":"All","price":-5},
          "bu bir nesne degil"
        ])");
        ClothingCollection col;
        LoadReport r = JsonManager::loadClothes(tmp, col);
        CHECK(r.ok());
        CHECK_EQ(r.loaded, 1);
        CHECK_EQ(r.skipped.size(), 5u);
        CHECK_EQ(col.size(), 1u);
        CHECK_EQ(col.findById(1)->name, "Iyi");
    }

    // 6) Kaydet -> yükle gidiş-dönüşü
    {
        ClothingCollection src;
        Clothing c;
        c.id = 42;
        c.name = "Test Ceket";
        c.category = Category::Outerwear;
        c.color = Color::Navy;
        c.style = Style::Formal;
        c.season = Season::Winter;
        c.price = 1999.5;
        c.imagePath = "images/ceket.jpg";
        c.isUserAdded = true;
        CHECK(src.add(c) == AddResult::Added);
        CHECK(JsonManager::saveClothes(tmp, src));

        ClothingCollection dst;
        LoadReport r = JsonManager::loadClothes(tmp, dst);
        CHECK(r.ok());
        CHECK_EQ(r.loaded, 1);
        const Clothing* back = dst.findById(42);
        CHECK(back != nullptr);
        if (back) {
            CHECK_EQ(back->name, "Test Ceket");
            CHECK(back->category == Category::Outerwear);
            CHECK(back->color == Color::Navy);
            CHECK(back->season == Season::Winter);
            CHECK_EQ(back->price, 1999.5);
            CHECK_EQ(back->imagePath, "images/ceket.jpg");
            CHECK_EQ(back->isUserAdded, true);
        }
    }

    // 7) Yazılamayan yol: false dönmeli
    {
        ClothingCollection col;
        CHECK(!JsonManager::saveClothes("olmayan_klasor/x.json", col));
    }

    std::remove(tmp.c_str());
    return finish("test_json_manager");
}