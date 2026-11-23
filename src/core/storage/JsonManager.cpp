#include "storage/JsonManager.h"

#include <fstream>
#include <nlohmann/json.hpp>

namespace style {
namespace {

using nlohmann::json;

// Tek bir JSON nesnesini Clothing'e çevirir. Hata varsa sebebi reason'a yazar.
bool parseClothing(const json& j, Clothing& out, std::string& reason) {
    if (!j.is_object()) {
        reason = "kayit bir nesne degil";
        return false;
    }

    // Zorunlu alanlar
    const char* required[] = {"id", "name", "category", "color",
                              "style", "season", "price"};
    for (const char* key : required) {
        if (!j.contains(key)) {
            reason = std::string("eksik alan: ") + key;
            return false;
        }
    }

    if (!j["id"].is_number_integer()) { reason = "id tam sayi degil"; return false; }
    if (!j["name"].is_string()) { reason = "name yazi degil"; return false; }
    if (!j["price"].is_number()) { reason = "price sayi degil"; return false; }
    for (const char* key : {"category", "color", "style", "season"}) {
        if (!j[key].is_string()) {
            reason = std::string(key) + " yazi degil";
            return false;
        }
    }

    auto cat = parseCategory(j["category"].get<std::string>());
    auto col = parseColor(j["color"].get<std::string>());
    auto sty = parseStyle(j["style"].get<std::string>());
    auto sea = parseSeason(j["season"].get<std::string>());
    if (!cat) { reason = "gecersiz category"; return false; }
    if (!col) { reason = "gecersiz color"; return false; }
    if (!sty) { reason = "gecersiz style"; return false; }
    if (!sea) { reason = "gecersiz season"; return false; }

    out.id = j["id"].get<int>();
    out.name = j["name"].get<std::string>();
    out.category = *cat;
    out.color = *col;
    out.style = *sty;
    out.season = *sea;
    out.price = j["price"].get<double>();

    // İsteğe bağlı alanlar: yoksa varsayılan, tipi yanlışsa yok say.
    if (j.contains("imagePath") && j["imagePath"].is_string())
        out.imagePath = j["imagePath"].get<std::string>();
    if (j.contains("productUrl") && j["productUrl"].is_string())
        out.productUrl = j["productUrl"].get<std::string>();
    if (j.contains("isUserAdded") && j["isUserAdded"].is_boolean())
        out.isUserAdded = j["isUserAdded"].get<bool>();

    return true;
}

}  // namespace

LoadReport JsonManager::loadClothes(const std::string& path,
                                    ClothingCollection& collection) {
    LoadReport report;

    std::ifstream file(path);
    if (!file.is_open()) {
        report.fatalError = "dosya acilamadi: " + path;
        return report;
    }

    json root;
    try {
        root = json::parse(file);
    } catch (const json::parse_error& e) {
        report.fatalError = std::string("bozuk JSON: ") + e.what();
        return report;
    }

    if (!root.is_array()) {
        report.fatalError = "kok eleman bir dizi olmali";
        return report;
    }

    int index = 0;
    for (const auto& element : root) {
        Clothing item;
        std::string reason;
        if (!parseClothing(element, item, reason)) {
            report.skipped.push_back({index, reason});
        } else {
            switch (collection.add(item)) {
                case AddResult::Added: ++report.loaded; break;
                case AddResult::InvalidItem:
                    report.skipped.push_back({index, "gecersiz kayit (id/isim/fiyat)"});
                    break;
                case AddResult::DuplicateId:
                    report.skipped.push_back({index, "tekrarlanan id"});
                    break;
            }
        }
        ++index;
    }
    return report;
}

bool JsonManager::saveClothes(const std::string& path,
                              const ClothingCollection& collection) {
    json root = json::array();
    for (const Clothing& c : collection.all()) {
        root.push_back({{"id", c.id},
                        {"name", c.name},
                        {"category", toString(c.category)},
                        {"color", toString(c.color)},
                        {"style", toString(c.style)},
                        {"season", toString(c.season)},
                        {"price", c.price},
                        {"imagePath", c.imagePath},
                        {"productUrl", c.productUrl},
                        {"isUserAdded", c.isUserAdded}});
    }

    std::ofstream file(path);
    if (!file.is_open()) return false;
    file << root.dump(2);
    return file.good();
}

}  // namespace style