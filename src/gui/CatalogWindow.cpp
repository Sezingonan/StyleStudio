#include "CatalogWindow.h"

#include <QColor>
#include <QComboBox>
#include <QCompleter>
#include <QDoubleSpinBox>
#include <QFrame>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPainter>
#include <QPushButton>
#include <QScrollArea>
#include <QSet>
#include <QStatusBar>
#include <QStringList>
#include <QStringListModel>
#include <QVBoxLayout>
#include <set>
#include <string>
#include <vector>

#include "managers/CatalogQuery.h"
#include "models/ClothingCollection.h"
#include "storage/JsonManager.h"

using namespace style;

namespace {

struct CatLabel { Category value; const char* text; };
const CatLabel kCats[] = {
    {Category::Top, "Üst giyim"},   {Category::Bottom, "Alt giyim"},
    {Category::Outerwear, "Dış giyim"}, {Category::Shoes, "Ayakkabı"},
    {Category::Bag, "Çanta"},       {Category::Accessory, "Aksesuar"},
};

struct ColLabel { Color value; const char* text; QRgb rgb; };
const ColLabel kCols[] = {
    {Color::White, "Beyaz", 0xF2EFEA},   {Color::Black, "Siyah", 0x1E1E1E},
    {Color::Gray, "Gri", 0x9A9A9A},      {Color::Beige, "Bej", 0xD8C3A5},
    {Color::Brown, "Kahverengi", 0x6B4A2F}, {Color::Blue, "Mavi", 0x4A7FB5},
    {Color::Navy, "Lacivert", 0x1F2D4D}, {Color::Red, "Kırmızı", 0xB5382E},
    {Color::Green, "Yeşil", 0x4F7F5A},   {Color::Pink, "Pembe", 0xE3A3B5},
    {Color::Yellow, "Sarı", 0xE2C044},   {Color::Other, "Diğer", 0xB0A89C},
};

QString catText(Category c) {
    for (const auto& e : kCats) if (e.value == c) return e.text;
    return "?";
}
QString colText(Color c) {
    for (const auto& e : kCols) if (e.value == c) return e.text;
    return "?";
}
QRgb colRgb(Color c) {
    for (const auto& e : kCols) if (e.value == c) return e.rgb;
    return 0xB0A89C;
}

const int kMaxCards = 60;
const int kColumns = 4;

}  // namespace

CatalogWindow::CatalogWindow(const QString& sourceDir, QWidget* parent)
    : QMainWindow(parent), sourceDir_(sourceDir) {
    // Çekirdek yükleme kodu Qt'siz; aynen kullanılıyor.
    ClothingCollection collection;
    LoadReport report = JsonManager::loadClothes(
        (sourceDir + "/data/clothes.json").toStdString(), collection);
    if (report.ok()) items_.loadFrom(collection);

    setWindowTitle("Style Studio");
    resize(1000, 750);
    setStyleSheet(
        "QMainWindow, QWidget#host { background-color: #F5F0E8; }"
        "QLabel { color: #3B2A20; }"
        "QLineEdit, QComboBox, QDoubleSpinBox { background: #FBF8F2; color: #3B2A20;"
        "  border: 1px solid #D9CBB8; border-radius: 6px; padding: 5px; }"
        "QFrame#card { background: #FBF8F2; border: 1px solid #D9CBB8; border-radius: 8px; }"
        "QPushButton { background: #3B2A20; color: #F5F0E8; border: none;"
        "  border-radius: 6px; padding: 6px 10px; }"
        "QPushButton:hover { background: #6B4A2F; }"
        "QScrollArea { border: none; }");

    QWidget* central = new QWidget;
    QVBoxLayout* root = new QVBoxLayout(central);

    // --- Başlık ve arama ---
    QLabel* title = new QLabel("Style Studio");
    title->setStyleSheet("font-size: 28px; font-weight: 600;");
    root->addWidget(title);

    search_ = new QLineEdit;
    search_->setPlaceholderText("Ürün ara (ör. pantolon, tişört)...");
    suggestModel_ = new QStringListModel(this);
    QCompleter* completer = new QCompleter(suggestModel_, this);
    completer->setCompletionMode(QCompleter::UnfilteredPopupCompletion);  // liste Trie'den geliyor
    search_->setCompleter(completer);
    root->addWidget(search_);

    // --- Filtre satırı ---
    QHBoxLayout* filters = new QHBoxLayout;
    category_ = new QComboBox;
    category_->addItem("Tüm kategoriler", -1);
    for (const auto& e : kCats) category_->addItem(e.text, static_cast<int>(e.value));
    color_ = new QComboBox;
    color_->addItem("Tüm renkler", -1);
    for (const auto& e : kCols) color_->addItem(e.text, static_cast<int>(e.value));

    minPrice_ = new QDoubleSpinBox;
    minPrice_->setRange(0, 100000);
    minPrice_->setDecimals(0);
    minPrice_->setSingleStep(100);
    minPrice_->setPrefix("Min ");
    minPrice_->setSuffix(" TL");
    maxPrice_ = new QDoubleSpinBox;
    maxPrice_->setRange(0, 100000);
    maxPrice_->setDecimals(0);
    maxPrice_->setSingleStep(100);
    maxPrice_->setPrefix("Max ");
    maxPrice_->setSuffix(" TL");
    maxPrice_->setSpecialValueText("Max sınırsız");  // 0 = sınır yok

    sort_ = new QComboBox;
    sort_->addItem("Sıralama: ID", static_cast<int>(SortOrder::None));
    sort_->addItem("Fiyat artan", static_cast<int>(SortOrder::PriceAsc));
    sort_->addItem("Fiyat azalan", static_cast<int>(SortOrder::PriceDesc));
    sort_->addItem("Ada göre (A-Z)", static_cast<int>(SortOrder::NameAsc));

    filters->addWidget(category_);
    filters->addWidget(color_);
    filters->addWidget(minPrice_);
    filters->addWidget(maxPrice_);
    filters->addWidget(sort_);
    root->addLayout(filters);

    resultLabel_ = new QLabel;
    root->addWidget(resultLabel_);

    // --- Kart ızgarası ---
    gridHost_ = new QWidget;
    gridHost_->setObjectName("host");
    grid_ = new QGridLayout(gridHost_);
    grid_->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    grid_->setSpacing(12);
    QScrollArea* scroll = new QScrollArea;
    scroll->setWidgetResizable(true);
    scroll->setWidget(gridHost_);
    root->addWidget(scroll, 1);

    setCentralWidget(central);

    // --- Olaylar ---
    connect(search_, &QLineEdit::textChanged, this, &CatalogWindow::updateSuggestions);
    connect(search_, &QLineEdit::textChanged, this, [this] { refresh(); });
    connect(category_, &QComboBox::currentIndexChanged, this, [this] { refresh(); });
    connect(color_, &QComboBox::currentIndexChanged, this, [this] { refresh(); });
    connect(sort_, &QComboBox::currentIndexChanged, this, [this] { refresh(); });
    connect(minPrice_, &QDoubleSpinBox::valueChanged, this, [this] { refresh(); });
    connect(maxPrice_, &QDoubleSpinBox::valueChanged, this, [this] { refresh(); });

    if (!report.ok()) {
        statusBar()->showMessage("Veri yüklenemedi: " + QString::fromStdString(report.fatalError));
    } else {
        statusBar()->showMessage(QString("%1 ürün yüklendi").arg(items_.size()));
    }
    refresh();
}

void CatalogWindow::updateSuggestions(const QString& text) {
    QStringList names;
    if (!text.trimmed().isEmpty()) {
        QSet<QString> seen;
        for (const Clothing& c : items_.suggestByName(text.toStdString(), 12)) {
            const QString n = QString::fromStdString(c.name);
            if (!seen.contains(n)) {
                seen.insert(n);
                names << n;
            }
        }
    }
    suggestModel_->setStringList(names);
}

void CatalogWindow::refresh() {
    // Filtreleri arayüzden oku
    CatalogFilter f;
    const int cat = category_->currentData().toInt();
    if (cat >= 0) f.category = static_cast<Category>(cat);
    const int col = color_->currentData().toInt();
    if (col >= 0) f.color = static_cast<Color>(col);
    if (minPrice_->value() > 0) f.minPrice = minPrice_->value();
    if (maxPrice_->value() > 0) f.maxPrice = maxPrice_->value();
    f.order = static_cast<SortOrder>(sort_->currentData().toInt());

    std::vector<Clothing> result = queryCatalog(items_, f);

    // Arama metni varsa Trie sonuçlarıyla kesiştir
    const QString text = search_->text().trimmed();
    if (!text.isEmpty()) {
        std::set<int> allowed;
        for (const Clothing& c : items_.suggestByName(text.toStdString(), items_.size()))
            allowed.insert(c.id);
        std::vector<Clothing> filtered;
        for (const Clothing& c : result)
            if (allowed.count(c.id)) filtered.push_back(c);
        result = std::move(filtered);
    }

    // Eski kartları sil
    while (QLayoutItem* item = grid_->takeAt(0)) {
        delete item->widget();
        delete item;
    }

    const int shown = static_cast<int>(std::min<std::size_t>(result.size(), kMaxCards));
    for (int i = 0; i < shown; ++i) {
        grid_->addWidget(makeCard(result[static_cast<std::size_t>(i)]), i / kColumns, i % kColumns);
    }

    QString info = QString("%1 ürün bulundu").arg(result.size());
    if (static_cast<int>(result.size()) > shown) info += QString(" (ilk %1 gösteriliyor)").arg(shown);
    resultLabel_->setText(info);
}

QPixmap CatalogWindow::cardPixmap(const Clothing& c) const {
    const int w = 170, h = 210;
    QPixmap photo(sourceDir_ + "/" + QString::fromStdString(c.imagePath));
    if (!photo.isNull()) {
        return photo.scaled(w, h, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    }
    // Fotoğraf yok: ürünün renginde yer tutucu
    QPixmap pm(w, h);
    const QColor bg(colRgb(c.color));
    pm.fill(bg);
    QPainter p(&pm);
    p.setPen(bg.lightness() < 128 ? QColor("#F5F0E8") : QColor("#3B2A20"));
    p.drawText(QRect(8, 8, w - 16, h - 16), Qt::AlignCenter | Qt::TextWordWrap,
               QString::fromStdString(c.name));
    return pm;
}

QWidget* CatalogWindow::makeCard(const Clothing& c) {
    QFrame* card = new QFrame;
    card->setObjectName("card");
    card->setFixedWidth(200);
    QVBoxLayout* lay = new QVBoxLayout(card);

    QLabel* img = new QLabel;
    img->setPixmap(cardPixmap(c));
    img->setAlignment(Qt::AlignCenter);
    lay->addWidget(img);

    QLabel* name = new QLabel(QString::fromStdString(c.name));
    name->setWordWrap(true);
    name->setStyleSheet("font-weight: 600;");
    lay->addWidget(name);

    lay->addWidget(new QLabel(catText(c.category) + " · " + colText(c.color)));
    lay->addWidget(new QLabel(QString::number(c.price, 'f', 0) + " TL"));

    QPushButton* add = new QPushButton("Kombine ekle");
    const int id = c.id;
    connect(add, &QPushButton::clicked, this, [this, id] {
        if (editor_.add(id))
            statusBar()->showMessage(QString("Kombine eklendi (%1 ürün)").arg(editor_.size()));
        else
            statusBar()->showMessage("Bu ürün zaten kombinde");
    });
    lay->addWidget(add);
    return card;
}