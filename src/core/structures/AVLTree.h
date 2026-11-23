#pragma once
#include <algorithm>
#include <cstddef>
#include <memory>
#include <utility>

namespace style {

// Kendini dengeleyen ikili arama ağacı (AVL).
// Key: karşılaştırılabilir anahtar (operator< gerekir), Value: anahtara bağlı veri.
template <typename Key, typename Value>
class AVLTree {
public:
    AVLTree() = default;

    // Kopyalama kapalı: unique_ptr zaten kopyalanamaz, bilerek açık yazıyoruz.
    AVLTree(const AVLTree&) = delete;
    AVLTree& operator=(const AVLTree&) = delete;
    AVLTree(AVLTree&&) = default;
    AVLTree& operator=(AVLTree&&) = default;

    std::size_t size() const { return size_; }
    bool empty() const { return size_ == 0; }

    // Ağacın yüksekliği: boş ağaç 0, tek düğüm 1.
    int height() const { return heightOf(root_.get()); }

private:
    struct Node {
        Key key;
        Value value;
        std::unique_ptr<Node> left;
        std::unique_ptr<Node> right;
        int height = 1;  // yeni düğüm yapraktır

        Node(Key k, Value v) : key(std::move(k)), value(std::move(v)) {}
    };

    // Boş (nullptr) düğümün yüksekliği 0 kabul edilir.
    static int heightOf(const Node* node) { return node ? node->height : 0; }

    // Denge faktörü = sol yükseklik - sağ yükseklik. Geçerli aralık: -1, 0, +1.
    static int balanceFactorOf(const Node* node) {
        return node ? heightOf(node->left.get()) - heightOf(node->right.get()) : 0;
    }

    // Çocuklar değişince düğümün yüksekliğini yeniden hesaplar.
    static void updateHeight(Node* node) {
        node->height = 1 + std::max(heightOf(node->left.get()),
                                    heightOf(node->right.get()));
    }

    std::unique_ptr<Node> root_;
    std::size_t size_ = 0;
};

}  // namespace style