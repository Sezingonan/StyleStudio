#include "models/ClothingCollection.h"

#include <algorithm>

namespace style {

AddResult ClothingCollection::add(const Clothing& item) {
    if (!item.isValid()) return AddResult::InvalidItem;
    if (findById(item.id) != nullptr) return AddResult::DuplicateId;
    items_.push_back(item);
    return AddResult::Added;
}

bool ClothingCollection::removeById(int id) {
    auto it = std::find_if(items_.begin(), items_.end(),
                           [id](const Clothing& c) { return c.id == id; });
    if (it == items_.end()) return false;
    items_.erase(it);
    return true;
}

const Clothing* ClothingCollection::findById(int id) const {
    auto it = std::find_if(items_.begin(), items_.end(),
                           [id](const Clothing& c) { return c.id == id; });
    return it == items_.end() ? nullptr : &(*it);
}

std::size_t ClothingCollection::size() const { return items_.size(); }
bool ClothingCollection::empty() const { return items_.empty(); }
const std::vector<Clothing>& ClothingCollection::all() const { return items_; }

}  // namespace style