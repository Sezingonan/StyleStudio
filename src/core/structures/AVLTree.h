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

    AVLTree(const AVLTree&) = delete;
    AVLTree& operator=(const AVLTree&) = delete;
    AVLTree(AVLTree&&) = default;
    AVLTree& operator=(AVLTree&&) = default;

    std::size_t size() const { return size_; }
    bool empty() const { return size_ == 0; }

    // Ağacın yüksekliği: boş ağaç 0, tek düğüm 1.
    int height() const { return heightOf(root_.get()); }

    // Ekler. Anahtar zaten varsa eklemez ve false döner.
    bool insert(const Key& key, const Value& value) {
        bool inserted = false;
        root_ = insertNode(std::move(root_), key, value, inserted);
        if (inserted) ++size_;
        return inserted;
    }

    // Anahtarı arar. Bulursa değere işaretçi, bulamazsa nullptr döner.
    // İşaretçi, ağaç değiştirilene (insert/erase) kadar geçerlidir.
    const Value* find(const Key& key) const {
        const Node* node = findNode(root_.get(), key);
        return node ? &node->value : nullptr;
    }

    Value* find(const Key& key) {
        return const_cast<Value*>(static_cast<const AVLTree*>(this)->find(key));
    }

    bool contains(const Key& key) const { return find(key) != nullptr; }

    // Anahtarları artan sırayla gezer, her düğüme fn(key, value) uygular.
    template <typename Func>
    void inOrder(Func fn) const {
        inOrderNode(root_.get(), fn);
    }

    // Test için: ağaçtaki her düğümde |denge faktörü| <= 1 mi?
    bool isBalanced() const { return checkBalanced(root_.get()); }

private:
    struct Node {
        Key key;
        Value value;
        std::unique_ptr<Node> left;
        std::unique_ptr<Node> right;
        int height = 1;  // yeni düğüm yapraktır

        Node(Key k, Value v) : key(std::move(k)), value(std::move(v)) {}
    };

    static int heightOf(const Node* node) { return node ? node->height : 0; }

    // Denge faktörü = sol yükseklik - sağ yükseklik. Geçerli aralık: -1, 0, +1.
    static int balanceFactorOf(const Node* node) {
        return node ? heightOf(node->left.get()) - heightOf(node->right.get()) : 0;
    }

    static void updateHeight(Node* node) {
        node->height = 1 + std::max(heightOf(node->left.get()),
                                    heightOf(node->right.get()));
    }

    // Sağa rotasyon: y'nin sol çocuğu x yukarı çıkar. Yeni kökü döndürür.
    static std::unique_ptr<Node> rotateRight(std::unique_ptr<Node> y) {
        std::unique_ptr<Node> x = std::move(y->left);
        y->left = std::move(x->right);
        updateHeight(y.get());
        x->right = std::move(y);
        updateHeight(x.get());
        return x;
    }

    // Sola rotasyon: x'in sağ çocuğu y yukarı çıkar. Yeni kökü döndürür.
    static std::unique_ptr<Node> rotateLeft(std::unique_ptr<Node> x) {
        std::unique_ptr<Node> y = std::move(x->right);
        x->right = std::move(y->left);
        updateHeight(x.get());
        y->left = std::move(x);
        updateHeight(y.get());
        return y;
    }

    // Düğümün yüksekliğini günceller, dengesizse uygun rotasyonu uygular.
    static std::unique_ptr<Node> rebalance(std::unique_ptr<Node> node) {
        updateHeight(node.get());
        int bf = balanceFactorOf(node.get());

        if (bf > 1) {  // sol taraf ağır
            if (balanceFactorOf(node->left.get()) < 0) {          // Sol-Sağ
                node->left = rotateLeft(std::move(node->left));
            }
            return rotateRight(std::move(node));                  // Sol-Sol
        }
        if (bf < -1) {  // sağ taraf ağır
            if (balanceFactorOf(node->right.get()) > 0) {         // Sağ-Sol
                node->right = rotateRight(std::move(node->right));
            }
            return rotateLeft(std::move(node));                   // Sağ-Sağ
        }
        return node;
    }

    static std::unique_ptr<Node> insertNode(std::unique_ptr<Node> node,
                                            const Key& key, const Value& value,
                                            bool& inserted) {
        if (!node) {
            inserted = true;
            return std::make_unique<Node>(key, value);
        }
        if (key < node->key) {
            node->left = insertNode(std::move(node->left), key, value, inserted);
        } else if (node->key < key) {
            node->right = insertNode(std::move(node->right), key, value, inserted);
        } else {
            return node;  // aynı anahtar: değişiklik yok
        }
        return rebalance(std::move(node));
    }

    // Döngüyle arama: her adımda sola ya da sağa inilir.
    static const Node* findNode(const Node* node, const Key& key) {
        while (node) {
            if (key < node->key) {
                node = node->left.get();
            } else if (node->key < key) {
                node = node->right.get();
            } else {
                return node;
            }
        }
        return nullptr;
    }

    template <typename Func>
    static void inOrderNode(const Node* node, Func& fn) {
        if (!node) return;
        inOrderNode(node->left.get(), fn);
        fn(node->key, node->value);
        inOrderNode(node->right.get(), fn);
    }

    static bool checkBalanced(const Node* node) {
        if (!node) return true;
        int bf = balanceFactorOf(node);
        if (bf < -1 || bf > 1) return false;
        return checkBalanced(node->left.get()) && checkBalanced(node->right.get());
    }

    std::unique_ptr<Node> root_;
    std::size_t size_ = 0;
};

}  // namespace style