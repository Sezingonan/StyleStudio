#include "text/Normalize.h"

#include <cstddef>

namespace style {
namespace {

// 2 byte'lık UTF-8 dizisini (lead, second) ASCII karşılığına çevirir.
// Eşleşme yoksa 0 döner.
char mapTurkish(unsigned char lead, unsigned char second) {
    if (lead == 0xC3) {
        switch (second) {
            case 0xA7: case 0x87: return 'c';  // ç Ç
            case 0xB6: case 0x96: return 'o';  // ö Ö
            case 0xBC: case 0x9C: return 'u';  // ü Ü
            default: return 0;
        }
    }
    if (lead == 0xC4) {
        switch (second) {
            case 0x9F: case 0x9E: return 'g';  // ğ Ğ
            case 0xB1: case 0xB0: return 'i';  // ı İ
            default: return 0;
        }
    }
    if (lead == 0xC5) {
        switch (second) {
            case 0x9F: case 0x9E: return 's';  // ş Ş
            default: return 0;
        }
    }
    return 0;
}

// UTF-8'de ilk byte'a bakarak karakterin kaç byte olduğunu bulur.
std::size_t sequenceLength(unsigned char lead) {
    if (lead >= 0xF0) return 4;
    if (lead >= 0xE0) return 3;
    if (lead >= 0xC0) return 2;
    return 1;  // başıboş devam byte'ı
}

}  // namespace

std::string normalize(const std::string& text) {
    std::string out;
    out.reserve(text.size());

    std::size_t i = 0;
    while (i < text.size()) {
        const unsigned char c = static_cast<unsigned char>(text[i]);

        if (c < 0x80) {  // ASCII
            if (c >= 'A' && c <= 'Z') {
                out += static_cast<char>(c - 'A' + 'a');
            } else {
                out += static_cast<char>(c);
            }
            ++i;
            continue;
        }

        // Non-ASCII: karakter uzunluğunu bul, metnin sonunu aşma.
        std::size_t len = sequenceLength(c);
        if (i + len > text.size()) len = text.size() - i;

        if (len == 2) {
            char mapped = mapTurkish(c, static_cast<unsigned char>(text[i + 1]));
            if (mapped != 0) out += mapped;
        }
        i += len;  // tanınmayan karakter atlanır
    }
    return out;
}

}  // namespace style