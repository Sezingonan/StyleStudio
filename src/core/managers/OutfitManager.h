#pragma once
#include <algorithm>
#include <cstddef>
#include <string>
#include <vector>

#include "managers/ClothingManager.h"
#include "models/ClothingCollection.h"  // AddResult
#include "models/Outfit.h"

namespace style {

// Kayıtlı kombinlerin deposu.
class OutfitManager {
public:
    // Yeni kombin kaydeder, ID'yi otomatik verir. Geçersizse 0 döner.
    int create(const std::string& name, const std::vector<int>& ids) {
        Outfit o;
        o.outfitId = nextId_;
        o.name = name;
        o.clothingIds = ids;
        if (!o.isValid()) return 0;
        outfits_.push_back(o);
        return nextId_++;
    }

    // Dosyadan okunan kombini kendi ID'siyle ekler.
    AddResult addExisting(const Outfit& o) {
        if (!o.isValid()) return AddResult::InvalidItem;
        if (find(o.outfitId) != nullptr) return AddResult::DuplicateId;
        outfits_.push_back(o);
        if (o.outfitId >= nextId_) nextId_ = o.outfitId + 1;
        return AddResult::Added;
    }

    // Var olan kombinin adını ve ürünlerini değiştirir. Geçersiz/bulunamazsa false.
    bool update(int id, const std::string& name, const std::vector<int>& ids) {
        Outfit* o = findMutable(id);
        if (!o) return false;
        Outfit candidate = *o;
        candidate.name = name;
        candidate.clothingIds = ids;
        if (!candidate.isValid()) return false;
        *o = candidate;
        return true;
    }

    bool remove(int id) {
        auto it = std::find_if(outfits_.begin(), outfits_.end(),
                               [id](const Outfit& o) { return o.outfitId == id; });
        if (it == outfits_.end()) return false;
        outfits_.erase(it);
        return true;
    }

    const Outfit* find(int id) const {
        auto it = std::find_if(outfits_.begin(), outfits_.end(),
                               [id](const Outfit& o) { return o.outfitId == id; });
        return it == outfits_.end() ? nullptr : &(*it);
    }

    const std::vector<Outfit>& all() const { return outfits_; }
    std::size_t size() const { return outfits_.size(); }
    bool empty() const { return outfits_.empty(); }

    // Kombindeki mevcut ürünlerin fiyat toplamı (silinmişler sayılmaz).
    static double totalPrice(const Outfit& o, const ClothingManager& items) {
        double sum = 0.0;
        for (int id : o.clothingIds) {
            if (const Clothing* c = items.findById(id)) sum += c->price;
        }
        return sum;
    }

    // Kombinde olup artık katalogda bulunmayan ürün sayısı.
    static std::size_t missingCount(const Outfit& o, const ClothingManager& items) {
        std::size_t missing = 0;
        for (int id : o.clothingIds) {
            if (items.findById(id) == nullptr) ++missing;
        }
        return missing;
    }

private:
    Outfit* findMutable(int id) {
        auto it = std::find_if(outfits_.begin(), outfits_.end(),
                               [id](const Outfit& o) { return o.outfitId == id; });
        return it == outfits_.end() ? nullptr : &(*it);
    }

    std::vector<Outfit> outfits_;
    int nextId_ = 1;  // silinen ID'ler tekrar kullanılmaz
};

}  // namespace style