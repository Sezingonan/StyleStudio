#include <iomanip>
#include <iostream>
#include <string>

#include "Version.h"
#include "managers/ClothingManager.h"
#include "models/ClothingCollection.h"
#include "storage/JsonManager.h"

using namespace style;

static void printItem(const Clothing& c) {
    std::cout << std::left << std::setw(6) << c.id << std::setw(22) << c.name
              << std::setw(11) << toString(c.category) << std::setw(8)
              << toString(c.color) << std::setw(9) << toString(c.style)
              << std::setw(8) << toString(c.season) << c.price << " TL\n";
}

int main(int argc, char* argv[]) {
    const std::string path = argc > 1 ? argv[1] : "data/clothes.json";

    std::cout << "Style Studio v" << version() << "\n";

    ClothingCollection collection;
    LoadReport report = JsonManager::loadClothes(path, collection);
    if (!report.ok()) {
        std::cout << "Yukleme basarisiz: " << report.fatalError << "\n"
                  << "Ipucu: programi proje klasorunden calistir.\n";
        return 1;
    }
    for (const SkippedRecord& s : report.skipped) {
        std::cout << "Atlanan kayit #" << s.index << ": " << s.reason << "\n";
    }

    ClothingManager manager;
    manager.loadFrom(collection);
    std::cout << manager.size() << " urun yuklendi.\n\n"
              << "Komutlar: list | id <no> | ara <onek> | cik\n";

    std::string line;
    while (std::cout << "> " && std::getline(std::cin, line)) {
        if (line == "cik") break;

        if (line == "list") {
            for (const Clothing& c : manager.all()) printItem(c);
        } else if (line.rfind("id ", 0) == 0) {
            try {
                const Clothing* c = manager.findById(std::stoi(line.substr(3)));
                if (c) printItem(*c); else std::cout << "Bulunamadi.\n";
            } catch (const std::exception&) {
                std::cout << "Gecersiz numara.\n";
            }
        } else if (line.rfind("ara ", 0) == 0) {
            auto found = manager.suggestByName(line.substr(4), 10);
            if (found.empty()) std::cout << "Sonuc yok.\n";
            for (const Clothing& c : found) printItem(c);
        } else if (!line.empty()) {
            std::cout << "Bilinmeyen komut.\n";
        }
    }
    return 0;
}