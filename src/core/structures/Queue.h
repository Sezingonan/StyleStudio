#pragma once
#include <cstddef>
#include <utility>

namespace style {

// FIFO kuyruk (tek yönlü bağlı liste). Kopyalama kapalı.
template <typename T>
class Queue {
public:
    Queue() = default;
    Queue(const Queue&) = delete;
    Queue& operator=(const Queue&) = delete;
    ~Queue() { clear(); }

    // Sona ekler. Zaman: O(1).
    void enqueue(const T& value) {
        Node* node = new Node{value, nullptr};
        if (tail_) tail_->next = node;
        else head_ = node;
        tail_ = node;
        ++size_;
    }

    // Baştaki elemanı out'a alıp çıkarır. Kuyruk boşsa false. Zaman: O(1).
    bool dequeue(T& out) {
        if (!head_) return false;
        Node* node = head_;
        out = std::move(node->value);
        head_ = node->next;
        if (!head_) tail_ = nullptr;
        delete node;
        --size_;
        return true;
    }

    // Baştaki eleman, boşsa nullptr. Sonraki dequeue'ya kadar geçerli.
    const T* front() const { return head_ ? &head_->value : nullptr; }

    std::size_t size() const { return size_; }
    bool empty() const { return size_ == 0; }

    // Döngüyle siler (uzun listede özyineleme yığını taşırırdı).
    void clear() {
        while (head_) {
            Node* next = head_->next;
            delete head_;
            head_ = next;
        }
        tail_ = nullptr;
        size_ = 0;
    }

private:
    struct Node {
        T value;
        Node* next;
    };
    Node* head_ = nullptr;
    Node* tail_ = nullptr;
    std::size_t size_ = 0;
};

}  // namespace style