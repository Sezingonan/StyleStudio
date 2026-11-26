#pragma once
#include <cstddef>
#include <utility>

namespace style {

// LIFO yığın (tek yönlü bağlı liste, tepe başta). Kopyalama kapalı.
template <typename T>
class Stack {
public:
    Stack() = default;
    Stack(const Stack&) = delete;
    Stack& operator=(const Stack&) = delete;
    ~Stack() { clear(); }

    // Tepeye ekler. Zaman: O(1).
    void push(const T& value) {
        top_ = new Node{value, top_};
        ++size_;
    }

    // Tepedeki elemanı out'a alıp çıkarır. Boşsa false. Zaman: O(1).
    bool pop(T& out) {
        if (!top_) return false;
        Node* node = top_;
        out = std::move(node->value);
        top_ = node->next;
        delete node;
        --size_;
        return true;
    }

    // Tepedeki eleman, boşsa nullptr. Sonraki push/pop'a kadar geçerli.
    const T* top() const { return top_ ? &top_->value : nullptr; }

    std::size_t size() const { return size_; }
    bool empty() const { return size_ == 0; }

    // Döngüyle siler (özyineleme yığını taşırmaz).
    void clear() {
        while (top_) {
            Node* next = top_->next;
            delete top_;
            top_ = next;
        }
        size_ = 0;
    }

private:
    struct Node {
        T value;
        Node* next;
    };
    Node* top_ = nullptr;
    std::size_t size_ = 0;
};

}  // namespace style