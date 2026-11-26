#pragma once
#include <cstddef>
#include <optional>
#include <set>
#include <vector>

#include "managers/ClothingManager.h"
#include "structures/Queue.h"

namespace style {

// Kullanıcının "sonra incelerim" dediği ürünlerin FIFO sırası.
// Aynı ürün kuyrukta bir kez bekleyebilir.
class ReviewQueue {
public:
    // Sona ekler. ID <= 0 ya da zaten kuyrukta ise false.
    bool enqueue(int id) {
        if (id <= 0 || pending_.count(id) > 0) return false;
        queue_.enqueue(id);
        pending_.insert(id);
        return true;
    }

    // Baştaki ID'yi çıkarır. Boşsa false.
    bool dequeue(int& out) {
        if (!queue_.dequeue(out)) return false;
        pending_.erase(out);
        return true;
    }

    // Sıradaki ürünü çıkarıp döner. Bu arada silinmiş ürünler atlanır.
    // Kuyruk boşalırsa std::nullopt.
    std::optional<Clothing> processNext(const ClothingManager& items) {
        int id = 0;
        while (dequeue(id)) {
            if (const Clothing* c = items.findById(id)) return *c;
        }
        return std::nullopt;
    }

    const int* front() const { return queue_.front(); }
    bool contains(int id) const { return pending_.count(id) > 0; }

    // Kuyruktaki ID'ler, baştan sona.
    std::vector<int> items() const {
        std::vector<int> out;
        queue_.forEach([&out](int id) { out.push_back(id); });
        return out;
    }

    std::size_t size() const { return queue_.size(); }
    bool empty() const { return queue_.empty(); }

    void clear() {
        queue_.clear();
        pending_.clear();
    }

private:
    Queue<int> queue_;
    std::set<int> pending_;  // tekrar kontrolü için
};

}  // namespace style