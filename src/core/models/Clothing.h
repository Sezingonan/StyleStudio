#pragma once
#include <string>

#include "models/Enums.h"

namespace style {

// Katalogdaki tek bir kıyafeti temsil eder.
// AVL, Trie ve Graph bu kaydın kopyasını değil, ID'sini tutacak.
struct Clothing {
    int id = 0;
    std::string name;
    Category category = Category::Top;
    Color color = Color::Other;
    Style style = Style::Casual;
    Season season = Season::All;
    double price = 0.0;
    std::string imagePath;
    std::string productUrl;  // isteğe bağlı, boş olabilir
    bool isUserAdded = false;

    // Kayıt kullanılabilir durumda mı?
    // ID > 0, isim boş değil, fiyat >= 0 olmalı.
    bool isValid() const {
        return id > 0 && !name.empty() && price >= 0.0;
    }
};

}  // namespace style