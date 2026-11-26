#pragma once
#include <string>

#include "managers/OutfitManager.h"
#include "storage/JsonManager.h"  // LoadReport, SkippedRecord

namespace style {

class OutfitStorage {
public:
    // outfits.json okur. Dosya yoksa fatalError dolu döner (ilk açılışta normal;
    // çağıran boş başlar). Hatalı kayıtlar atlanıp report.skipped'a yazılır.
    static LoadReport load(const std::string& path, OutfitManager& manager);

    // Tüm kombinleri dosyaya yazar. Başarılıysa true.
    static bool save(const std::string& path, const OutfitManager& manager);
};

}  // namespace style