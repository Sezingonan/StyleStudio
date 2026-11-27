#pragma once
#include <vector>

#include "managers/ClothingManager.h"
#include "managers/OutfitEditor.h"

namespace style {

enum class PlaceResult { Added, Replaced, Rejected };

// Aksesuar dışındaki kategoriler 1, aksesuar en fazla 3 ürün.
inline std::size_t categoryLimit(Category c) {
    return c == Category::Accessory ? 3 : 1;
}

// Ürünü kombine kategori kuralına göre yerleştirir:
//  - kategoride yer varsa ekler (Added)
//  - tek kontenjanlı kategoride başka ürün varsa onu çıkarıp yenisini koyar (Replaced)
//  - aksesuar doluysa ya da ürün zaten kombindeyse/bulunamazsa reddeder (Rejected)
// Her sonuç Undo için tek adım değil, değişiklik başına bir adım kaydeder
// (Replaced = çıkar + ekle = iki Undo adımı).
inline PlaceResult placeInOutfit(OutfitEditor& editor, const ClothingManager& items, int id) {
    const Clothing* c = items.findById(id);
    if (!c || editor.contains(id)) return PlaceResult::Rejected;

    std::vector<int> sameCategory;
    for (int existing : editor.items()) {
        const Clothing* e = items.findById(existing);
        if (e && e->category == c->category) sameCategory.push_back(existing);
    }

    if (sameCategory.size() < categoryLimit(c->category)) {
        return editor.add(id) ? PlaceResult::Added : PlaceResult::Rejected;
    }
    if (c->category == Category::Accessory) return PlaceResult::Rejected;

    editor.remove(sameCategory.front());
    return editor.add(id) ? PlaceResult::Replaced : PlaceResult::Rejected;
}

}  // namespace style