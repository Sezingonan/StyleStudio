#include "models/Enums.h"

#include <algorithm>
#include <array>
#include <cctype>
#include <cstddef>
#include <utility>

namespace style {
namespace {

// Her enum için "değer <-> yazı" eşleştirme tablosu.
template <typename E>
using Entry = std::pair<E, const char*>;

const std::array<Entry<Category>, 6> kCategories{{
    {Category::Top, "Top"},
    {Category::Bottom, "Bottom"},
    {Category::Outerwear, "Outerwear"},
    {Category::Shoes, "Shoes"},
    {Category::Bag, "Bag"},
    {Category::Accessory, "Accessory"},
}};

const std::array<Entry<Color>, 12> kColors{{
    {Color::White, "White"},   {Color::Black, "Black"},
    {Color::Gray, "Gray"},     {Color::Beige, "Beige"},
    {Color::Brown, "Brown"},   {Color::Blue, "Blue"},
    {Color::Navy, "Navy"},     {Color::Red, "Red"},
    {Color::Green, "Green"},   {Color::Pink, "Pink"},
    {Color::Yellow, "Yellow"}, {Color::Other, "Other"},
}};

const std::array<Entry<Style>, 5> kStyles{{
    {Style::Casual, "Casual"},
    {Style::Formal, "Formal"},
    {Style::Sport, "Sport"},
    {Style::Street, "Street"},
    {Style::Classic, "Classic"},
}};

const std::array<Entry<Season>, 5> kSeasons{{
    {Season::Spring, "Spring"},
    {Season::Summer, "Summer"},
    {Season::Autumn, "Autumn"},
    {Season::Winter, "Winter"},
    {Season::All, "All"},
}};

std::string toLower(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(), [](unsigned char ch) {
        return static_cast<char>(std::tolower(ch));
    });
    return s;
}

template <typename E, std::size_t N>
std::string nameOf(const std::array<Entry<E>, N>& table, E value) {
    for (const auto& entry : table) {
        if (entry.first == value) return entry.second;
    }
    return "Unknown";
}

template <typename E, std::size_t N>
std::optional<E> parseFrom(const std::array<Entry<E>, N>& table,
                           const std::string& text) {
    const std::string key = toLower(text);
    for (const auto& entry : table) {
        if (toLower(entry.second) == key) return entry.first;
    }
    return std::nullopt;
}

}  // namespace

std::string toString(Category value) { return nameOf(kCategories, value); }
std::string toString(Color value) { return nameOf(kColors, value); }
std::string toString(Style value) { return nameOf(kStyles, value); }
std::string toString(Season value) { return nameOf(kSeasons, value); }

std::optional<Category> parseCategory(const std::string& text) {
    return parseFrom(kCategories, text);
}
std::optional<Color> parseColor(const std::string& text) {
    return parseFrom(kColors, text);
}
std::optional<Style> parseStyle(const std::string& text) {
    return parseFrom(kStyles, text);
}
std::optional<Season> parseSeason(const std::string& text) {
    return parseFrom(kSeasons, text);
}

}  // namespace style