#include <vector>

#include "test_framework.h"
#include "managers/ReviewQueue.h"

using namespace style;
using Ids = std::vector<int>;

static Clothing make(int id, const char* name) {
    Clothing c;
    c.id = id;
    c.name = name;
    c.price = 100;
    return c;
}

int main() {
    // Boş kuyruk
    {
        ReviewQueue q;
        ClothingManager m;
        int x = -1;
        CHECK(q.empty());
        CHECK(q.front() == nullptr);
        CHECK(!q.dequeue(x));
        CHECK(!q.processNext(m).has_value());
    }

    // FIFO sırası
    {
        ReviewQueue q;
        CHECK(q.enqueue(30)); CHECK(q.enqueue(10)); CHECK(q.enqueue(20));
        CHECK((q.items() == Ids{30, 10, 20}));  // eklenme sırası, ID sırası değil
        CHECK_EQ(*q.front(), 30);
        int x = 0;
        CHECK(q.dequeue(x)); CHECK_EQ(x, 30);
        CHECK(q.dequeue(x)); CHECK_EQ(x, 10);
        CHECK((q.items() == Ids{20}));
    }

    // Tekrar ve geçersiz ID reddedilir
    {
        ReviewQueue q;
        CHECK(q.enqueue(5));
        CHECK(!q.enqueue(5));
        CHECK(!q.enqueue(0));
        CHECK(!q.enqueue(-3));
        CHECK_EQ(q.size(), 1u);
        CHECK(q.contains(5));
        CHECK(!q.contains(6));
    }

    // İşlenen ürün tekrar sıraya girebilir
    {
        ReviewQueue q;
        q.enqueue(5);
        int x = 0;
        q.dequeue(x);
        CHECK(!q.contains(5));
        CHECK(q.enqueue(5));
    }

    // processNext: ürünü getirir, silinmiş ürünleri atlar
    {
        ClothingManager m;
        m.add(make(1, "Tisort"));
        m.add(make(2, "Pantolon"));
        m.add(make(3, "Bot"));

        ReviewQueue q;
        q.enqueue(1); q.enqueue(2); q.enqueue(3);
        m.remove(2);  // kuyrukta beklerken silindi

        auto a = q.processNext(m);
        CHECK(a.has_value());
        if (a) CHECK_EQ(a->id, 1);
        auto b = q.processNext(m);  // 2 atlanır
        CHECK(b.has_value());
        if (b) CHECK_EQ(b->id, 3);
        CHECK(!q.processNext(m).has_value());
        CHECK(q.empty());
    }

    // Hepsi silinmişse nullopt ve kuyruk boşalır
    {
        ClothingManager m;
        ReviewQueue q;
        q.enqueue(7); q.enqueue(8);
        CHECK(!q.processNext(m).has_value());
        CHECK(q.empty());
    }

    // clear
    {
        ReviewQueue q;
        q.enqueue(1); q.enqueue(2);
        q.clear();
        CHECK(q.empty());
        CHECK(q.enqueue(1));  // tekrar kontrolü de sıfırlanmış olmalı
    }

    return finish("test_review_queue");
}