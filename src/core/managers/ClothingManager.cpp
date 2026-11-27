#include "managers/ClothingManager.h"

#include <set>

namespace style {
namespace {

// Adın kendisi ve her kelimeden başlayan sonekleri:
// "Siyah Pantolon" -> "Siyah Pantolon", "Pantolon".
std::vector<std::string> nameKeys(const std::string& name) {
    std::vector<std::string> keys{name};
    for (std::size_t i = 0; i + 1 < name.size(); ++i) {
        if (name[i] == ' ' && name[i + 1] != ' ') keys.push_back(name.substr(i + 1));
    }
    return keys;
}

}  // namespace

AddResult ClothingManager::add(const Clothing& item) {
    if (!item.isValid()) return AddResult::InvalidItem;
    if (store_.contains(item.id)) return AddResult::DuplicateId;

    // Önce tam ad: reddederse AVL'ye ve diğer anahtarlara dokunmadan çıkarız.
    const std::vector<std::string> keys = nameKeys(item.name);
    if (!nameIndex_.insert(keys[0], item.id)) return AddResult::InvalidItem;
    for (std::size_t i = 1; i < keys.size(); ++i) nameIndex_.insert(keys[i], item.id);

    store_.insert(item.id, item);
    return AddResult::Added;
}

bool ClothingManager::remove(int id) {
    const Clothing* found = store_.find(id);
    if (!found) return false;

    // erase sonrası işaretçi geçersiz olur, adı önce kopyala.
    const std::string name = found->name;
    for (const std::string& key : nameKeys(name)) nameIndex_.remove(key, id);
    store_.erase(id);
    return true;
}

const Clothing* ClothingManager::findById(int id) const {
    return store_.find(id);
}

std::vector<Clothing> ClothingManager::suggestByName(const std::string& prefix,
                                                     std::size_t k) const {
    std::vector<Clothing> result;
    if (k == 0) return result;

    // Aynı ürün birden çok anahtardan gelebilir: tekrarları ele, k'dan az
    // benzersiz ürün kalırsa Trie'den daha fazlasını iste.
    std::size_t want = k;
    while (true) {
        const std::vector<int> ids = nameIndex_.suggest(prefix, want);
        std::set<int> seen;
        std::vector<int> unique;
        for (int id : ids) {
            if (seen.insert(id).second) unique.push_back(id);
        }
        if (unique.size() >= k || ids.size() < want) {
            if (unique.size() > k) unique.resize(k);
            for (int id : unique) {
                if (const Clothing* c = store_.find(id)) result.push_back(*c);
            }
            return result;
        }
        want *= 2;
    }
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