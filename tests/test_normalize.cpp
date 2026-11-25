#include "test_framework.h"
#include "text/Normalize.h"

using namespace style;

int main() {
    // ASCII küçültme
    CHECK_EQ(normalize("Beyaz Tisort"), "beyaz tisort");
    CHECK_EQ(normalize("ABC 123"), "abc 123");

    // Türkçe harfler
    CHECK_EQ(normalize("Tişört"), "tisort");
    CHECK_EQ(normalize("ÇAĞRI"), "cagri");
    CHECK_EQ(normalize("İSTANBUL"), "istanbul");
    CHECK_EQ(normalize("ıspanak"), "ispanak");
    CHECK_EQ(normalize("Şık Ürün"), "sik urun");
    CHECK_EQ(normalize("Göğüs"), "gogus");

    // Sınır durumları
    CHECK_EQ(normalize(""), "");
    CHECK_EQ(normalize("   "), "   ");          // boşluk korunur
    CHECK_EQ(normalize("a\xF0\x9F\x98\x80" "b"), "ab");  // emoji atılır
    CHECK_EQ(normalize("abc\xC3"), "abc");      // yarım kalan karakter çökmemeli

    return finish("test_normalize");
}