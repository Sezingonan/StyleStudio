#pragma once
#include <string>
#include <vector>

namespace style {

// Kayıtlı bir kombin. Ürünlerin kendisini değil ID'lerini tutar.
struct Outfit {
    int outfitId = 0;
    std::string name;
    std::vector<int> clothingIds;

    // ID > 0, isim boş değil, en az bir ürün, ürün ID'leri > 0 ve tekrarsız.
    bool isValid() const {
        if (outfitId <= 0 || name.empty() || clothingIds.empty()) return false;
        for (std::size_t i = 0; i < clothingIds.size(); ++i) {
            if (clothingIds[i] <= 0) return false;
            for (std::size_t j = i + 1; j < clothingIds.size(); ++j) {
                if (clothingIds[i] == clothingIds[j]) return false;
            }
        }
        return true;
    }
};

}  // namespace style