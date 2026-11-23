#pragma once
#include <optional>
#include <string>

namespace style {

enum class Category { Top, Bottom, Outerwear, Shoes, Bag, Accessory };

enum class Color {
    White, Black, Gray, Beige, Brown, Blue, Navy, Red, Green, Pink, Yellow, Other
};

enum class Style { Casual, Formal, Sport, Street, Classic };

enum class Season { Spring, Summer, Autumn, Winter, All };

// Enum -> yazı (JSON'a yazarken ve ekranda gösterirken kullanılır)
std::string toString(Category value);
std::string toString(Color value);
std::string toString(Style value);
std::string toString(Season value);

// Yazı -> enum (JSON'dan okurken kullanılır).
// Tanınmayan yazı gelirse std::nullopt döner, program çökmez.
// Büyük/küçük harf fark etmez: "black", "Black", "BLACK" hepsi geçerli.
std::optional<Category> parseCategory(const std::string& text);
std::optional<Color> parseColor(const std::string& text);
std::optional<Style> parseStyle(const std::string& text);
std::optional<Season> parseSeason(const std::string& text);

}  // namespace style