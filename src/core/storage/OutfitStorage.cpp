#include "storage/OutfitStorage.h"

#include <fstream>
#include <nlohmann/json.hpp>

namespace style {
namespace {

using nlohmann::json;

bool parseOutfit(const json& j, Outfit& out, std::string& reason) {
    if (!j.is_object()) { reason = "kayit bir nesne degil"; return false; }
    for (const char* key : {"outfitId", "name", "clothingIds"}) {
        if (!j.contains(key)) {
            reason = std::string("eksik alan: ") + key;
            return false;
        }
    }
    if (!j["outfitId"].is_number_integer()) { reason = "outfitId tam sayi degil"; return false; }
    if (!j["name"].is_string()) { reason = "name yazi degil"; return false; }
    if (!j["clothingIds"].is_array()) { reason = "clothingIds dizi degil"; return false; }

    out.outfitId = j["outfitId"].get<int>();
    out.name = j["name"].get<std::string>();
    out.clothingIds.clear();
    for (const auto& e : j["clothingIds"]) {
        if (!e.is_number_integer()) { reason = "clothingIds icinde tam sayi olmayan eleman"; return false; }
        out.clothingIds.push_back(e.get<int>());
    }
    return true;
}

}  // namespace

LoadReport OutfitStorage::load(const std::string& path, OutfitManager& manager) {
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
        Outfit o;
        std::string reason;
        if (!parseOutfit(element, o, reason)) {
            report.skipped.push_back({index, reason});
        } else {
            switch (manager.addExisting(o)) {
                case AddResult::Added: ++report.loaded; break;
                case AddResult::InvalidItem:
                    report.skipped.push_back({index, "gecersiz kombin (id/isim/urunler)"});
                    break;
                case AddResult::DuplicateId:
                    report.skipped.push_back({index, "tekrarlanan outfitId"});
                    break;
            }
        }
        ++index;
    }
    return report;
}

bool OutfitStorage::save(const std::string& path, const OutfitManager& manager) {
    json root = json::array();
    for (const Outfit& o : manager.all()) {
        root.push_back({{"outfitId", o.outfitId},
                        {"name", o.name},
                        {"clothingIds", o.clothingIds}});
    }
    std::ofstream file(path);
    if (!file.is_open()) return false;
    file << root.dump(2);
    return file.good();
}

}  // namespace style