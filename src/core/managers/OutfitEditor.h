#pragma once
#include <algorithm>
#include <cstddef>
#include <vector>

#include "structures/Stack.h"

namespace style {

// Üzerinde çalışılan kombin (ürün ID listesi) ve Undo/Redo geçmişi.
// Geçmiş, kombinin kopyalarını (snapshot) iki Stack'te tutar.
class OutfitEditor {
public:
    // Ürünü ekler. ID <= 0 ya da zaten kombinde ise false (geçmişe yazılmaz).
    bool add(int id) {
        if (id <= 0 || contains(id)) return false;
        checkpoint();
        items_.push_back(id);
        return true;
    }

    // Ürünü çıkarır. Kombinde yoksa false (geçmişe yazılmaz).
    bool remove(int id) {
        auto it = std::find(items_.begin(), items_.end(), id);
        if (it == items_.end()) return false;
        checkpoint();
        items_.erase(it);
        return true;
    }

    // Kombini boşaltır. Zaten boşsa false. Undo ile geri alınabilir.
    bool clear() {
        if (items_.empty()) return false;
        checkpoint();
        items_.clear();
        return true;
    }

    // Son değişikliği geri alır. Geri alınacak bir şey yoksa false.
    bool undo() {
        std::vector<int> previous;
        if (!undo_.pop(previous)) return false;
        redo_.push(items_);
        items_ = std::move(previous);
        return true;
    }

    // Geri alınan değişikliği tekrar uygular. Yoksa false.
    bool redo() {
        std::vector<int> next;
        if (!redo_.pop(next)) return false;
        undo_.push(items_);
        items_ = std::move(next);
        return true;
    }

    bool canUndo() const { return !undo_.empty(); }
    bool canRedo() const { return !redo_.empty(); }

    bool contains(int id) const {
        return std::find(items_.begin(), items_.end(), id) != items_.end();
    }
    const std::vector<int>& items() const { return items_; }
    std::size_t size() const { return items_.size(); }
    bool empty() const { return items_.empty(); }

private:
    // Değişiklikten ÖNCE çağrılır: mevcut hâli kaydeder, ileri geçmişi siler.
    void checkpoint() {
        undo_.push(items_);
        redo_.clear();
    }

    std::vector<int> items_;
    Stack<std::vector<int>> undo_;
    Stack<std::vector<int>> redo_;
};

}  // namespace style