#pragma once
#include <string>

namespace style {

// Arama için metni sadeleştirir (UTF-8 girdi):
//  - A-Z harfleri küçültür
//  - Türkçe harfleri ASCII'ye indirger: ç ğ ı i̇ ö ş ü (büyükleriyle birlikte)
//  - Tanınmayan non-ASCII karakterleri (emoji vb.) atar
// Boşluk, rakam ve noktalama olduğu gibi kalır.
std::string normalize(const std::string& text);

}  // namespace style