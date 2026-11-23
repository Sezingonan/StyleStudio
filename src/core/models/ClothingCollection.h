#pragma once
#include <cstddef>
#include <vector>

#include "models/Clothing.h"

namespace style {

// Ekleme işleminin sonucu.
enum class AddResult { Added, InvalidItem, DuplicateId };

// Tüm ürünlerin tek sahibi. Diğer yapılar (AVL, Trie, Graph) ID tutar.
class ClothingCollection {
public:
    // Geçersiz kayıt (isValid() false) ya da var olan bir ID reddedilir.
    AddResult add(const Clothing& item);

    // ID'si verilen ürünü siler. Bulunamazsa false döner.
    bool removeById(int id);

    // Bulamazsa nullptr. İşaretçi, sonraki add/remove çağrısına kadar geçerlidir.
    const Clothing* findById(int id) const;

    std::size_t size() const;
    bool empty() const;
    const std::vector<Clothing>& all() const;

private:
    std::vector<Clothing> items_;
};

}  // namespace style