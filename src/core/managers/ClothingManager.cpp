#include "managers/ClothingManager.h"

namespace style {

AddResult ClothingManager::add(const Clothing& item) {
    if (!item.isValid()) return AddResult::InvalidItem;
    if (store_.contains(item.id)) return AddResult::DuplicateId;

    // Önce Trie: reddederse AVL'ye dokunmadan çıkarız.
    if (!nameIndex_.insert(item.name, item.id)) return AddResult::InvalidItem;

    store_.insert(item.id, item);
    return AddResult::Added;
}

bool ClothingManager::remove(int id) {
    const Clothing* found = store_.find(id);
    if (!found) return false;

    // erase sonrası işaretçi geçersiz olur, adı önce kopyala.
    const std::string name = found->name;
    nameIndex_.remove(name, id);
    store_.erase(id);
    return true;
}

const Clothing* ClothingManager::findById(int id) const {
    return store_.find(id);
}

std::vector<Clothing> ClothingManager::suggestByName(const std::string& prefix,
                                                     std::size_t k) const {
    std::vector<Clothing> result;
    for (int id : nameIndex_.suggest(prefix, k)) {
        if (const Clothing* c = store_.find(id)) result.push_back(*c);
    }
    return result;
}

std::vector<Clothing> ClothingManager::all() const {
    std::vector<Clothing> result;
    result.reserve(store_.size());
    store_.inOrder([&result](int, const Clothing& c) { result.push_back(c); });
    return result;
}

std::size_t ClothingManager::loadFrom(const ClothingCollection& collection) {
    std::size_t added = 0;
    for (const Clothing& c : collection.all()) {
        if (add(c) == AddResult::Added) ++added;
    }
    return added;
}

}  // namespace style