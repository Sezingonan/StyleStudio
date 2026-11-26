#include "test_framework.h"
#include "structures/Stack.h"

using namespace style;

int main() {
    // Boş yığın
    {
        Stack<int> s;
        int x = -1;
        CHECK(s.empty());
        CHECK_EQ(s.size(), 0u);
        CHECK(s.top() == nullptr);
        CHECK(!s.pop(x));  // boşta pop çökmemeli
        CHECK_EQ(x, -1);
    }

    // LIFO sırası
    {
        Stack<int> s;
        s.push(1); s.push(2); s.push(3);
        CHECK_EQ(s.size(), 3u);
        CHECK_EQ(*s.top(), 3);
        int x = 0;
        CHECK(s.pop(x)); CHECK_EQ(x, 3);
        CHECK(s.pop(x)); CHECK_EQ(x, 2);
        CHECK_EQ(*s.top(), 1);
        CHECK(s.pop(x)); CHECK_EQ(x, 1);
        CHECK(s.empty());
        CHECK(!s.pop(x));
    }

    // Boşaldıktan sonra yeniden kullanım
    {
        Stack<int> s;
        int x = 0;
        s.push(5);
        s.pop(x);
        s.push(6);
        CHECK_EQ(*s.top(), 6);
        CHECK_EQ(s.size(), 1u);
    }

    // clear
    {
        Stack<int> s;
        for (int i = 0; i < 10; ++i) s.push(i);
        s.clear();
        CHECK(s.empty());
        CHECK(s.top() == nullptr);
    }

    // Büyük yığın: destructor yığını taşırmamalı
    {
        Stack<int> s;
        for (int i = 0; i < 200000; ++i) s.push(i);
        CHECK_EQ(s.size(), 200000u);
        CHECK_EQ(*s.top(), 199999);
    }

    return finish("test_stack");
}