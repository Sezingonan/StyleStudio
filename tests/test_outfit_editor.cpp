#include <vector>

#include "test_framework.h"
#include "managers/OutfitEditor.h"

using namespace style;
using Ids = std::vector<int>;

int main() {
    // Boş editör
    {
        OutfitEditor e;
        CHECK(e.empty());
        CHECK(!e.canUndo());
        CHECK(!e.canRedo());
        CHECK(!e.undo());   // çökmemeli
        CHECK(!e.redo());
        CHECK(!e.remove(1));
        CHECK(!e.clear());
    }

    // Senaryo: tişört, pantolon, çanta ekle; undo; redo
    {
        OutfitEditor e;
        CHECK(e.add(1)); CHECK(e.add(2)); CHECK(e.add(3));
        CHECK((e.items() == Ids{1, 2, 3}));

        CHECK(e.undo());
        CHECK((e.items() == Ids{1, 2}));   // çanta gitti
        CHECK(e.canRedo());

        CHECK(e.redo());
        CHECK((e.items() == Ids{1, 2, 3})); // çanta geri geldi
        CHECK(!e.canRedo());
    }

    // Çoklu undo, sonra çoklu redo
    {
        OutfitEditor e;
        e.add(1); e.add(2); e.add(3);
        CHECK(e.undo()); CHECK(e.undo()); CHECK(e.undo());
        CHECK(e.empty());
        CHECK(!e.undo());                  // başlangıçtan öteye gidilmez
        CHECK(e.redo()); CHECK(e.redo()); CHECK(e.redo());
        CHECK((e.items() == Ids{1, 2, 3}));
        CHECK(!e.redo());
    }

    // Undo sonrası yeni işlem redo geçmişini silmeli
    {
        OutfitEditor e;
        e.add(1); e.add(2);
        e.undo();                          // {1}, redo'da {1,2}
        CHECK(e.canRedo());
        CHECK(e.add(9));                   // yeni dal
        CHECK(!e.canRedo());
        CHECK((e.items() == Ids{1, 9}));
        CHECK(!e.redo());
    }

    // Başarısız işlemler geçmişe yazılmamalı
    {
        OutfitEditor e;
        e.add(1);
        CHECK(!e.add(1));                  // tekrar
        CHECK(!e.add(0));                  // geçersiz id
        CHECK(!e.add(-5));
        CHECK(!e.remove(99));              // kombinde yok
        CHECK(e.undo());                   // tek adım yeterli: sadece add(1) kayıtlı
        CHECK(e.empty());
        CHECK(!e.canUndo());
    }

    // Remove ve undo
    {
        OutfitEditor e;
        e.add(1); e.add(2); e.add(3);
        CHECK(e.remove(2));
        CHECK((e.items() == Ids{1, 3}));
        CHECK(e.undo());
        CHECK((e.items() == Ids{1, 2, 3})); // sıra da geri gelmeli
    }

    // Clear ve undo
    {
        OutfitEditor e;
        e.add(1); e.add(2);
        CHECK(e.clear());
        CHECK(e.empty());
        CHECK(e.undo());
        CHECK((e.items() == Ids{1, 2}));
        CHECK(e.redo());
        CHECK(e.empty());
    }

    // Uzun geçmiş: 1000 ekle, hepsini geri al, hepsini ileri al
    {
        OutfitEditor e;
        for (int i = 1; i <= 1000; ++i) e.add(i);
        CHECK_EQ(e.size(), 1000u);
        int undone = 0;
        while (e.undo()) ++undone;
        CHECK_EQ(undone, 1000);
        CHECK(e.empty());
        int redone = 0;
        while (e.redo()) ++redone;
        CHECK_EQ(redone, 1000);
        CHECK_EQ(e.size(), 1000u);
    }

    return finish("test_outfit_editor");
}