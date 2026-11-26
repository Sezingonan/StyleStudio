#pragma once
#include <algorithm>
#include <cstdlib>

#include "models/Clothing.h"

namespace style {

namespace detail {

// Renk çarkındaki dilim (0-11). Nötr renkler için -1.
inline int hueOf(Color c) {
    switch (c) {
        case Color::Yellow: return 0;
        case Color::Beige:
        case Color::Brown:  return 2;
        case Color::Red:
        case Color::Pink:   return 4;
        case Color::Blue:
        case Color::Navy:   return 8;
        case Color::Green:  return 10;
        default:            return -1;  // White, Black, Gray, Other
    }
}

// Stil grubu: 0 = Casual/Street/Sport, 1 = Formal/Classic.
inline int styleGroupOf(Style s) {
    return (s == Style::Formal || s == Style::Classic) ? 1 : 0;
}

}  // namespace detail

// Renk çarkı kuralı: nötr her şeyle uyumlu; aynı dilim (ton),
// yan yana (en fazla 2 dilim) ya da karşılıklı (6 dilim) uyumlu.
inline bool colorsCompatible(Color a, Color b) {
    const int ha = detail::hueOf(a);
    const int hb = detail::hueOf(b);
    if (ha < 0 || hb < 0) return true;
    int d = std::abs(ha - hb);
    d = std::min(d, 12 - d);  // çember üzerinde en kısa mesafe
    return d <= 2 || d == 6;
}

inline bool stylesCompatible(Style a, Style b) {
    return detail::styleGroupOf(a) == detail::styleGroupOf(b);
}

inline bool categoriesCompatible(Category a, Category b) { return a != b; }

// Üç kuralın hepsi sağlanmalı. Bir ürün kendisiyle uyumlu sayılmaz.
inline bool areCompatible(const Clothing& a, const Clothing& b) {
    return a.id != b.id && categoriesCompatible(a.category, b.category) &&
           colorsCompatible(a.color, b.color) && stylesCompatible(a.style, b.style);
}

}  // namespace style