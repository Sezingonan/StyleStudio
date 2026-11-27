#pragma once
#include <cstddef>
#include <string>
#include <vector>

#include "models/Clothing.h"
#include "models/ClothingCollection.h"  // AddResult için
#include "structures/AVLTree.h"
#include "structures/Trie.h"

namespace style {

// Ürünlerin tek sahibi (AVL) ve ad indeksi (Trie) tek kapıdan yönetilir.
// Ekleme/silme her iki yapıyı birlikte günceller.
class ClothingManager {
public:
    // Geçersiz kayıt, tekrarlı ID ya da arama için kullanılamayan ad
    // (normalize sonrası boş) reddedilir; reddedilince hiçbir yapı değişmez.
    AddResult add(const Clothing& item);

    // ID'si verilen ürünü her iki yapıdan siler. Yoksa false.
    bool remove(int id);

    // Bulamazsa nullptr. İşaretçi, sonraki add/remove çağrısına kadar geçerlidir.
    const Clothing* findById(int id) const;

    // Ad önekiyle başlayan en fazla k ürün (alfabetik ad sırası). Ürün kopyaları döner.
    std::vector<Clothing> suggestByName(const std::string& prefix,
                                        std::size_t k) const;

    // Tüm ürünler, ID'ye göre artan sırada (AVL in-order).
    std::vector<Clothing> all() const;

    // Her ürüne ID sırasıyla fn(const Clothing&) uygular, kopyalama yapmaz.
    // fn içinde add/remove çağrılmamalıdır.
    template <typename Func>
    void forEach(Func fn) const {
        store_.inOrder([&fn](int, const Clothing& c) { fn(c); });
    }

    // Koleksiyondaki ürünleri ekler. Başarıyla eklenen sayısını döner.
    std::size_t loadFrom(const ClothingCollection& collection);

    std::size_t size() const { return store_.size(); }
    bool empty() const { return store_.empty(); }

    // AVL ağacının yüksekliği (denge kanıtı için).
    int treeHeight() const { return store_.height(); }

private:
    AVLTree<int, Clothing> store_;  // id -> ürün (tek kayıt)
    Trie nameIndex_;                // ad -> id
};

}  // namespace style