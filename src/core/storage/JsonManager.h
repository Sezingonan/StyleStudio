#pragma once
#include <string>
#include <vector>

#include "models/ClothingCollection.h"

namespace style {

// Atlanan bir kaydın bilgisi.
struct SkippedRecord {
    int index;           // dizideki sırası (0'dan başlar)
    std::string reason;  // neden atlandı
};

// Yükleme sonucu.
struct LoadReport {
    int loaded = 0;                     // başarıyla eklenen kayıt sayısı
    std::vector<SkippedRecord> skipped;  // atlanan kayıtlar
    std::string fatalError;             // boş değilse dosya hiç yüklenemedi
    bool ok() const { return fatalError.empty(); }
};

class JsonManager {
public:
    // clothes.json okur, geçerli kayıtları collection'a ekler.
    static LoadReport loadClothes(const std::string& path,
                                  ClothingCollection& collection);

    // collection içeriğini dosyaya yazar. Başarılıysa true.
    static bool saveClothes(const std::string& path,
                            const ClothingCollection& collection);
};

}  // namespace style