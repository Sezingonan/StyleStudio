#include "test_framework.h"
#include "structures/Queue.h"

using namespace style;

int main() {
    // Boş kuyruk
    {
        Queue<int> q;
        int x = -1;
        CHECK(q.empty());
        CHECK_EQ(q.size(), 0u);
        CHECK(q.front() == nullptr);
        CHECK(!q.dequeue(x));  // boşta çıkarma çökmemeli
        CHECK_EQ(x, -1);       // out değişmemeli
    }

    // FIFO sırası
    {
        Queue<int> q;
        q.enqueue(1); q.enqueue(2); q.enqueue(3);
        CHECK_EQ(q.size(), 3u);
        CHECK_EQ(*q.front(), 1);
        int x = 0;
        CHECK(q.dequeue(x)); CHECK_EQ(x, 1);
        CHECK(q.dequeue(x)); CHECK_EQ(x, 2);
        CHECK_EQ(*q.front(), 3);
        CHECK(q.dequeue(x)); CHECK_EQ(x, 3);
        CHECK(q.empty());
        CHECK(!q.dequeue(x));
    }

    // Boşaldıktan sonra yeniden kullanım (tail doğru sıfırlanmalı)
    {
        Queue<int> q;
        int x = 0;
        q.enqueue(5);
        q.dequeue(x);
        q.enqueue(6);
        q.enqueue(7);
        CHECK(q.dequeue(x)); CHECK_EQ(x, 6);
        CHECK(q.dequeue(x)); CHECK_EQ(x, 7);
        CHECK(q.empty());
    }

    // Araya karışık enqueue/dequeue
    {
        Queue<int> q;
        int x = 0;
        q.enqueue(1); q.enqueue(2);
        q.dequeue(x); CHECK_EQ(x, 1);
        q.enqueue(3);
        q.dequeue(x); CHECK_EQ(x, 2);
        q.dequeue(x); CHECK_EQ(x, 3);
    }

    // clear
    {
        Queue<int> q;
        for (int i = 0; i < 10; ++i) q.enqueue(i);
        q.clear();
        CHECK(q.empty());
        CHECK(q.front() == nullptr);
        q.enqueue(1);
        CHECK_EQ(q.size(), 1u);
    }

    // Büyük kuyruk: sıra korunmalı, destructor yığını taşırmamalı
    {
        Queue<int> q;
        for (int i = 0; i < 200000; ++i) q.enqueue(i);
        CHECK_EQ(q.size(), 200000u);
        int x = 0;
        bool ordered = true;
        for (int i = 0; i < 100000; ++i) {
            q.dequeue(x);
            if (x != i) ordered = false;
        }
        CHECK(ordered);
        // 100000 eleman kuyrukta kalarak yok edilir
    }

    return finish("test_queue");
}