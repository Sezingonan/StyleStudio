#pragma once
#include <algorithm>
#include <cstddef>
#include <map>
#include <memory>
#include <string>
#include <utility>
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

    // (ad, id) çiftini siler. Çift yoksa false döner.
    // Boşalan düğümler budanır, böylece silinen adın yolu ağaçta kalmaz.
    bool remove(const std::string& name, int id) {
        const std::string key = normalize(name);
        if (key.empty()) return false;

        // Kökten başlayıp (ebeveyn, harf) çiftlerini kaydederek in.
        std::vector<std::pair<Node*, char>> path;
        Node* node = &root_;
        for (char ch : key) {
            auto it = node->children.find(ch);
            if (it == node->children.end()) return false;
            path.emplace_back(node, ch);
            node = it->second.get();
        }

        auto pos = std::find(node->ids.begin(), node->ids.end(), id);
        if (pos == node->ids.end()) return false;
        node->ids.erase(pos);
        --size_;

        // Aşağıdan yukarı: ID'si de çocuğu da olmayan düğümü ebeveyninden kopar.
        for (std::size_t i = path.size(); i-- > 0;) {
            Node* parent = path[i].first;
            char ch = path[i].second;
            const Node* child = parent->children.find(ch)->second.get();
            if (child->ids.empty() && child->children.empty()) {
                parent->children.erase(ch);
            } else {
                break;  // bu düğüm hâlâ gerekli, üsttekiler de gerekli
            }
        }
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

    // Bu önekle başlayan adların ID'lerinden en fazla k tanesini döner.
    // Sıra: adlar alfabetik (kısa ad önce), aynı adlı ürünler ekleme sırasıyla.
    // Boş önek: tüm adlardan başlar. k == 0 ise boş liste.
    std::vector<int> suggest(const std::string& prefix, std::size_t k) const {
        std::vector<int> result;
        if (k == 0) return result;

        const std::string key = normalize(prefix);
        const Node* start = key.empty() ? &root_ : walk(key);
        if (!start) return result;

        collect(start, k, result);
        return result;
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

    // Derinlik öncelikli gezinti: önce düğümün kendi ID'leri, sonra çocuklar.
    // out.size() k'ya ulaşınca durur.
    static void collect(const Node* node, std::size_t k, std::vector<int>& out) {
        for (int id : node->ids) {
            if (out.size() >= k) return;
            out.push_back(id);
        }
        for (const auto& entry : node->children) {
            if (out.size() >= k) return;
            collect(entry.second.get(), k, out);
        }
    }

    Node root_;
    std::size_t size_ = 0;
};

}  // namespace style