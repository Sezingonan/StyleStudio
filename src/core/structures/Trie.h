#pragma once
#include <algorithm>
#include <cstddef>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include "text/Normalize.h"

namespace style {

// Ürün adlarını harf harf saklayan ön ek ağacı (Trie).
// Her ad normalize() ile sadeleştirilip eklenir; kelimenin bittiği
// düğümde o adı taşıyan ürünlerin ID'leri tutulur.
class Trie {
public:
    // (ad, id) çiftini ekler. Ad normalize sonrası boşsa ya da aynı
    // çift zaten varsa eklemez ve false döner.
    bool insert(const std::string& name, int id) {
        const std::string key = normalize(name);
        if (key.empty()) return false;

        Node* node = &root_;
        for (char ch : key) {
            std::unique_ptr<Node>& child = node->children[ch];
            if (!child) child = std::make_unique<Node>();
            node = child.get();
        }

        if (std::find(node->ids.begin(), node->ids.end(), id) != node->ids.end()) {
            return false;
        }
        node->ids.push_back(id);
        ++size_;
        return true;
    }

    // Bu önekle başlayan en az bir kayıtlı ad var mı?
    // Boş önek: Trie boş değilse true.
    bool startsWith(const std::string& prefix) const {
        const std::string key = normalize(prefix);
        if (key.empty()) return size_ > 0;
        return walk(key) != nullptr;
    }

    // Adın tam eşleşmesi: o adı taşıyan ID'ler, yoksa nullptr.
    const std::vector<int>* find(const std::string& name) const {
        const std::string key = normalize(name);
        if (key.empty()) return nullptr;
        const Node* node = walk(key);
        if (!node || node->ids.empty()) return nullptr;
        return &node->ids;
    }

    // Kayıtlı (ad, id) çifti sayısı.
    std::size_t size() const { return size_; }
    bool empty() const { return size_ == 0; }

private:
    struct Node {
        std::map<char, std::unique_ptr<Node>> children;
        std::vector<int> ids;  // bu düğümde biten adları taşıyan ürünler
    };

    // Kökten başlayıp key harflerini izler. Yol yoksa nullptr.
    const Node* walk(const std::string& key) const {
        const Node* node = &root_;
        for (char ch : key) {
            auto it = node->children.find(ch);
            if (it == node->children.end()) return nullptr;
            node = it->second.get();
        }
        return node;
    }

    Node root_;
    std::size_t size_ = 0;
};

}  // namespace style