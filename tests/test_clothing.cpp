#include "test_framework.h"
#include "models/Clothing.h"

using namespace style;

int main() {
    // Varsayılan kayıt geçersiz olmalı (id = 0, isim boş)
    Clothing empty;
    CHECK(!empty.isValid());

    // Doğru doldurulmuş kayıt geçerli olmalı
    Clothing c;
    c.id = 1001;
    c.name = "Palazzo Pantolon";
    c.category = Category::Bottom;
    c.color = Color::Black;
    c.price = 1200.0;
    CHECK(c.isValid());
    CHECK_EQ(c.isUserAdded, false);

    // Sınır durumları
    Clothing free = c;
    free.price = 0.0;
    CHECK(free.isValid());  // 0 TL geçerli

    Clothing negativePrice = c;
    negativePrice.price = -5.0;
    CHECK(!negativePrice.isValid());

    Clothing noName = c;
    noName.name = "";
    CHECK(!noName.isValid());

    Clothing badId = c;
    badId.id = -3;
    CHECK(!badId.isValid());

    // Kopyalama bağımsız olmalı
    Clothing copy = c;
    copy.name = "Başka";
    CHECK_EQ(c.name, "Palazzo Pantolon");

    return finish("test_clothing");
}