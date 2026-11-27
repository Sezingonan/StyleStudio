#ifdef _WIN32
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#include <cstddef>
#include <algorithm>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>


#include "Version.h"
#include "algorithms/BinarySearch.h"
#include "algorithms/QuickSort.h"
#include "managers/CatalogQuery.h"
#include "managers/ClothingManager.h"
#include "managers/OutfitEditor.h"
#include "managers/OutfitManager.h"
#include "managers/RecommendationManager.h"
#include "managers/ReviewQueue.h"
#include "models/ClothingCollection.h"
#include "storage/JsonManager.h"
#include "storage/OutfitStorage.h"
#include "text/Normalize.h"

using namespace style;

namespace {

const char* kOutfitsPath = "data/outfits.json";

// UTF-8 metni karakter sayısına göre boşlukla doldurur (setw bayt sayar).
std::string pad(const std::string& s, std::size_t width) {
    std::size_t len = 0;
    for (unsigned char ch : s) {
        if ((ch & 0xC0) != 0x80) ++len;  // devam baytlarını sayma
    }
    std::string out = s;
    if (len < width) out.append(width - len, ' ');
    return out;
}

void printItem(const Clothing& c) {
    std::cout << std::left << std::setw(6) << c.id << pad(c.name, 30)
              << std::setw(11) << toString(c.category) << std::setw(8)
              << toString(c.color) << std::setw(9) << toString(c.style)
              << std::setw(8) << toString(c.season) << c.price << " TL\n";
}

void printList(const std::vector<Clothing>& v, std::size_t maxRows = 25) {
    if (v.empty()) {
        std::cout << "Sonuc yok.\n";
        return;
    }
    const std::size_t shown = std::min(v.size(), maxRows);
    for (std::size_t i = 0; i < shown; ++i) printItem(v[i]);
    if (v.size() > shown)
        std::cout << "... ilk " << shown << " urun gosterildi, toplam " << v.size() << ".\n";
    else
        std::cout << v.size() << " urun.\n";
}

bool toInt(const std::string& s, int& out) {
    try {
        std::size_t pos = 0;
        out = std::stoi(s, &pos);
        return pos == s.size();
    } catch (const std::exception&) {
        return false;
    }
}

bool toDouble(const std::string& s, double& out) {
    try {
        std::size_t pos = 0;
        out = std::stod(s, &pos);
        return pos == s.size();
    } catch (const std::exception&) {
        return false;
    }
}

std::vector<std::string> split(const std::string& line) {
    std::istringstream in(line);
    std::vector<std::string> tokens;
    std::string t;
    while (in >> t) tokens.push_back(t);
    return tokens;
}

// tokens[from..] birlestirilir (bosluklu kombin adlari icin).
std::string joinFrom(const std::vector<std::string>& t, std::size_t from) {
    std::string out;
    for (std::size_t i = from; i < t.size(); ++i) {
        if (!out.empty()) out += ' ';
        out += t[i];
    }
    return out;
}

struct App {
    ClothingManager items;
    RecommendationManager recommender{items};
    OutfitManager outfits;
    OutfitEditor editor;
    ReviewQueue reviewQueue;
};

void printHelp() {
    std::cout <<
        "Komutlar:\n"
        "  list | id <no> | ara <onek>\n"
        "  filtre kategori=.. renk=.. stil=.. sezon=.. min=.. max=.. sirala=fiyat-artan|fiyat-azalan|ad\n"
        "  oner <id> [seviye]\n"
        "  fiyat <tutar> | fiyat <min> <max>   (Binary Search)\n"
        "  bilgi                               (AVL yuksekligi, graf)\n"
        "  kombin ekle <id> | cikar <id> | goster | temizle | geri | ileri\n"
        "  kombin kaydet <ad> | ac <no> | kombinler\n"
        "  sira ekle <id> | goster | isle\n"
        "  yardim | cik\n";
}

void cmdFilter(const std::vector<std::string>& t, const App& app) {
    CatalogFilter f;
    for (std::size_t i = 1; i < t.size(); ++i) {
        const std::size_t eq = t[i].find('=');
        if (eq == std::string::npos) {
            std::cout << "Gecersiz kosul: " << t[i] << " (anahtar=deger bekleniyor)\n";
            return;
        }
        const std::string key = normalize(t[i].substr(0, eq));
        const std::string val = t[i].substr(eq + 1);
        bool ok = true;
        if (key == "kategori") {
            auto v = parseCategory(val);
            if (v) f.category = *v; else ok = false;
        } else if (key == "renk") {
            auto v = parseColor(val);
            if (v) f.color = *v; else ok = false;
        } else if (key == "stil") {
            auto v = parseStyle(val);
            if (v) f.style = *v; else ok = false;
        } else if (key == "sezon") {
            auto v = parseSeason(val);
            if (v) f.season = *v; else ok = false;
        } else if (key == "min") {
            double d = 0;
            if (toDouble(val, d)) f.minPrice = d; else ok = false;
        } else if (key == "max") {
            double d = 0;
            if (toDouble(val, d)) f.maxPrice = d; else ok = false;
        } else if (key == "sirala") {
            const std::string v = normalize(val);
            if (v == "fiyat-artan") f.order = SortOrder::PriceAsc;
            else if (v == "fiyat-azalan") f.order = SortOrder::PriceDesc;
            else if (v == "ad") f.order = SortOrder::NameAsc;
            else ok = false;
        } else {
            std::cout << "Bilinmeyen anahtar: " << key << "\n";
            return;
        }
        if (!ok) {
            std::cout << "Gecersiz deger: " << t[i] << "\n";
            return;
        }
    }
        printList(queryCatalog(app.items, f));
    if (f.minPrice || f.maxPrice)
        std::cout << "[Fiyat araligi: Quick Sort + Binary Search]\n";
    if (f.order == SortOrder::PriceAsc || f.order == SortOrder::PriceDesc)
        std::cout << "[Siralama: Quick Sort]\n";
    else if (f.order == SortOrder::NameAsc)
        std::cout << "[Siralama: Merge Sort (kararli)]\n";
}

void cmdRecommend(const std::vector<std::string>& t, const App& app) {
    int id = 0;
    int level = 2;
    if (t.size() < 2 || !toInt(t[1], id) || (t.size() > 2 && !toInt(t[2], level))) {
        std::cout << "Kullanim: oner <id> [seviye]\n";
        return;
    }
    const Clothing* start = app.items.findById(id);
    if (!start) {
        std::cout << "Urun bulunamadi.\n";
        return;
    }
    std::cout << "Secilen: ";
    printItem(*start);
        auto list = app.recommender.recommendByCategory(id, level, 3);
    if (list.empty()) std::cout << "Uyumlu urun bulunamadi.\n";
    Category current = Category::Top;
    bool first = true;
    for (const auto& s : list) {
        if (first || s.item.category != current) {
            std::cout << "  " << toString(s.item.category) << ":\n";
            current = s.item.category;
            first = false;
        }
        std::cout << "    [seviye " << s.level << "] ";
        printItem(s.item);
    }
}

void showEditor(const App& app) {
    if (app.editor.empty()) {
        std::cout << "Kombin bos.\n";
        return;
    }
    double total = 0;
    for (int id : app.editor.items()) {
        if (const Clothing* c = app.items.findById(id)) {
            printItem(*c);
            total += c->price;
        } else {
            std::cout << id << "  (silinmis urun)\n";
        }
    }
    std::cout << "Toplam: " << total << " TL\n";
}

void cmdOutfit(const std::vector<std::string>& t, App& app) {
    if (t.size() < 2) {
        std::cout << "Kullanim: kombin ekle|cikar|goster|temizle|geri|ileri|kaydet|ac|kombinler\n";
        return;
    }
    const std::string& sub = t[1];
    int id = 0;

    if (sub == "ekle") {
        if (t.size() < 3 || !toInt(t[2], id)) { std::cout << "Kullanim: kombin ekle <id>\n"; return; }
        if (!app.items.findById(id)) { std::cout << "Urun bulunamadi.\n"; return; }
        if (!app.editor.add(id)) { std::cout << "Urun zaten kombinde.\n"; return; }
        showEditor(app);
    } else if (sub == "cikar") {
        if (t.size() < 3 || !toInt(t[2], id)) { std::cout << "Kullanim: kombin cikar <id>\n"; return; }
        if (!app.editor.remove(id)) { std::cout << "Urun kombinde degil.\n"; return; }
        showEditor(app);
    } else if (sub == "goster") {
        showEditor(app);
    } else if (sub == "temizle") {
        if (!app.editor.clear()) std::cout << "Kombin zaten bos.\n";
        else std::cout << "Kombin temizlendi (geri alinabilir).\n";
    } else if (sub == "geri") {
        if (!app.editor.undo()) { std::cout << "Geri alinacak islem yok.\n"; return; }
        showEditor(app);
    } else if (sub == "ileri") {
        if (!app.editor.redo()) { std::cout << "Ileri alinacak islem yok.\n"; return; }
        showEditor(app);
    } else if (sub == "kaydet") {
        const std::string name = joinFrom(t, 2);
        const int newId = app.outfits.create(name, app.editor.items());
        if (newId == 0) {
            std::cout << "Kaydedilemedi (ad ve en az bir urun gerekli).\n";
            return;
        }
        if (OutfitStorage::save(kOutfitsPath, app.outfits))
            std::cout << "Kombin #" << newId << " kaydedildi.\n";
        else
            std::cout << "Kombin bellekte tutuldu ama dosyaya yazilamadi.\n";
    } else if (sub == "ac") {
        if (t.size() < 3 || !toInt(t[2], id)) { std::cout << "Kullanim: kombin ac <no>\n"; return; }
        const Outfit* o = app.outfits.find(id);
        if (!o) { std::cout << "Kombin bulunamadi.\n"; return; }
        app.editor.clear();
        for (int cid : o->clothingIds) {
            if (app.items.findById(cid)) app.editor.add(cid);
        }
        std::cout << "\"" << o->name << "\" acildi.\n";
        showEditor(app);
    } else if (sub == "kombinler") {
        if (app.outfits.empty()) { std::cout << "Kayitli kombin yok.\n"; return; }
        for (const Outfit& o : app.outfits.all()) {
            std::cout << "#" << o.outfitId << "  " << o.name << "  ["
                      << o.clothingIds.size() << " urun, "
                      << OutfitManager::totalPrice(o, app.items) << " TL";
            const std::size_t missing = OutfitManager::missingCount(o, app.items);
            if (missing > 0) std::cout << ", " << missing << " eksik";
            std::cout << "]\n";
        }
    } else {
        std::cout << "Bilinmeyen kombin komutu.\n";
    }
}

void cmdQueue(const std::vector<std::string>& t, App& app) {
    if (t.size() < 2) {
        std::cout << "Kullanim: sira ekle <id> | goster | isle\n";
        return;
    }
    const std::string& sub = t[1];
    int id = 0;
    if (sub == "ekle") {
        if (t.size() < 3 || !toInt(t[2], id)) { std::cout << "Kullanim: sira ekle <id>\n"; return; }
        if (!app.items.findById(id)) { std::cout << "Urun bulunamadi.\n"; return; }
        if (!app.reviewQueue.enqueue(id)) { std::cout << "Urun zaten sirada.\n"; return; }
        std::cout << "Siraya eklendi (" << app.reviewQueue.size() << " urun bekliyor).\n";
    } else if (sub == "goster") {
        if (app.reviewQueue.empty()) { std::cout << "Sira bos.\n"; return; }
        int n = 1;
        for (int qid : app.reviewQueue.items()) {
            std::cout << n++ << ". ";
            if (const Clothing* c = app.items.findById(qid)) printItem(*c);
            else std::cout << qid << " (silinmis)\n";
        }
    } else if (sub == "isle") {
        auto next = app.reviewQueue.processNext(app.items);
        if (!next) { std::cout << "Sira bos.\n"; return; }
        std::cout << "Simdi inceleniyor: ";
        printItem(*next);
        std::cout << "Kalan: " << app.reviewQueue.size() << "\n";
    } else {
        std::cout << "Bilinmeyen sira komutu.\n";
    }
}
void cmdInfo(const App& app) {
    const std::size_t n = app.items.size();
    const int bound = static_cast<int>(1.44 * std::log2(static_cast<double>(n) + 2.0));
    std::cout << "Urun sayisi        : " << n << "\n"
              << "AVL yuksekligi     : " << app.items.treeHeight()
              << "  (teorik ust sinir ~" << bound << ", dengesiz agacta en kotu "
              << n << ")\n"
              << "Graf               : " << app.recommender.graph().nodeCount()
              << " dugum, " << app.recommender.graph().edgeCount() << " kenar\n"
              << "Kayitli kombin     : " << app.outfits.size() << "\n"
              << "Inceleme sirasi    : " << app.reviewQueue.size() << "\n";
}

// fiyat <tutar>  ya da  fiyat <min> <max>
void cmdPrice(const std::vector<std::string>& t, const App& app) {
    double lo = 0, hi = 0;
    if (t.size() < 2 || t.size() > 3 || !toDouble(t[1], lo) ||
        (t.size() == 3 && !toDouble(t[2], hi))) {
        std::cout << "Kullanim: fiyat <tutar>  veya  fiyat <min> <max>\n";
        return;
    }
    if (t.size() == 2) hi = lo;
    if (lo > hi) {
        std::cout << "min, max'tan buyuk olamaz.\n";
        return;
    }
    std::vector<Clothing> v = app.items.all();
    quickSort(v, detail::PriceLess{});
    if (!isSorted(v, detail::PriceLess{})) {
        std::cout << "Siralama dogrulanamadi.\n";
        return;
    }
    const std::size_t first = lowerBound(v, lo, detail::PriceLess{});
    const std::size_t last = upperBound(v, hi, detail::PriceLess{});
    std::cout << "Quick Sort ile fiyata gore siralandi (" << v.size()
              << " urun). Binary Search: alt sinir indeksi " << first
              << ", ust sinir indeksi " << last << " -> " << (last - first)
              << " urun.\n";
    const std::size_t shown = std::min<std::size_t>(last - first, 20);
    for (std::size_t i = 0; i < shown; ++i) printItem(v[first + i]);
    if (last - first > shown) std::cout << "... (ilk 20 gosterildi)\n";
}
}  // namespace

int main(int argc, char* argv[]) {
	#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);  // Türkçe karakterler bozulmasın
    SetConsoleCP(CP_UTF8);        // Türkçe karakterli komut yazabilmek için
#endif
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

    App app;
    app.items.loadFrom(collection);
    app.recommender.rebuild();
    std::cout << app.items.size() << " urun yuklendi, "
              << app.recommender.graph().edgeCount() << " uyumluluk baglantisi kuruldu.\n";

    LoadReport outfitReport = OutfitStorage::load(kOutfitsPath, app.outfits);
    if (outfitReport.ok()) {
        std::cout << app.outfits.size() << " kayitli kombin yuklendi.\n";
        for (const SkippedRecord& s : outfitReport.skipped)
            std::cout << "Atlanan kombin #" << s.index << ": " << s.reason << "\n";
    } else if (outfitReport.fatalError.rfind("dosya acilamadi", 0) != 0) {
        std::cout << "Kombin dosyasi okunamadi: " << outfitReport.fatalError << "\n";
    }

    printHelp();

    std::string line;
    while (std::cout << "> " && std::getline(std::cin, line)) {
        const std::vector<std::string> t = split(line);
        if (t.empty()) continue;
        const std::string& cmd = t[0];

        if (cmd == "cik") break;
        else if (cmd == "yardim") printHelp();        
	else if (cmd == "list")
            printList(app.items.all(),
                      (t.size() > 1 && t[1] == "tum") ? static_cast<std::size_t>(-1) : 25);
        else if (cmd == "id") {
            int id = 0;
            if (t.size() < 2 || !toInt(t[1], id)) std::cout << "Kullanim: id <no>\n";
            else if (const Clothing* c = app.items.findById(id)) printItem(*c);
            else std::cout << "Bulunamadi.\n";
        } else if (cmd == "ara") {
            printList(app.items.suggestByName(joinFrom(t, 1), 10));
        } else if (cmd == "filtre") cmdFilter(t, app);
        else if (cmd == "oner") cmdRecommend(t, app);
        else if (cmd == "bilgi") cmdInfo(app);
        else if (cmd == "fiyat") cmdPrice(t, app);
        else if (cmd == "kombin") cmdOutfit(t, app);
        else if (cmd == "kombinler") cmdOutfit({"kombin", "kombinler"}, app);
        else if (cmd == "sira") cmdQueue(t, app);
        else std::cout << "Bilinmeyen komut. 'yardim' yaz.\n";
    }
    return 0;
}