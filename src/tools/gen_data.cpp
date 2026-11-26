#include <iostream>
#include <string>

#include "storage/JsonManager.h"
#include "storage/SampleData.h"

// Kullanim: style_gen [adet] [cikti_dosyasi] [tohum]
int main(int argc, char* argv[]) {
    unsigned long count = 60;
    unsigned long seed = 42;
    std::string path = "data/clothes.json";

    try {
        if (argc > 1) count = std::stoul(argv[1]);
        if (argc > 2) path = argv[2];
        if (argc > 3) seed = std::stoul(argv[3]);
    } catch (const std::exception&) {
        std::cout << "Kullanim: style_gen [adet] [cikti_dosyasi] [tohum]\n";
        return 1;
    }
    if (count == 0 || count > 100000) {
        std::cout << "Adet 1 ile 100000 arasinda olmali.\n";
        return 1;
    }

    style::ClothingCollection data =
        style::generateSampleClothes(count, static_cast<unsigned>(seed));
    if (!style::JsonManager::saveClothes(path, data)) {
        std::cout << "Dosya yazilamadi: " << path << "\n";
        return 1;
    }
    std::cout << data.size() << " urun yazildi: " << path << "\n";
    return 0;
}