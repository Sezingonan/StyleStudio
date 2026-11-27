#pragma once
#include <QColor>
#include <QPainter>
#include <QPixmap>
#include <QRgb>
#include <QString>

#include "models/Clothing.h"

// Katalog ve kombin panosunun ortak yardımcıları.
namespace gui {

struct CatLabel { style::Category value; const char* text; };
inline const CatLabel kCats[] = {
    {style::Category::Top, "Üst giyim"},       {style::Category::Bottom, "Alt giyim"},
    {style::Category::Outerwear, "Dış giyim"}, {style::Category::Shoes, "Ayakkabı"},
    {style::Category::Bag, "Çanta"},           {style::Category::Accessory, "Aksesuar"},
};

struct ColLabel { style::Color value; const char* text; QRgb rgb; };
inline const ColLabel kCols[] = {
    {style::Color::White, "Beyaz", 0xF2EFEA},       {style::Color::Black, "Siyah", 0x1E1E1E},
    {style::Color::Gray, "Gri", 0x9A9A9A},          {style::Color::Beige, "Bej", 0xD8C3A5},
    {style::Color::Brown, "Kahverengi", 0x6B4A2F},  {style::Color::Blue, "Mavi", 0x4A7FB5},
    {style::Color::Navy, "Lacivert", 0x1F2D4D},     {style::Color::Red, "Kırmızı", 0xB5382E},
    {style::Color::Green, "Yeşil", 0x4F7F5A},       {style::Color::Pink, "Pembe", 0xE3A3B5},
    {style::Color::Yellow, "Sarı", 0xE2C044},       {style::Color::Other, "Diğer", 0xB0A89C},
};

inline QString catText(style::Category c) {
    for (const auto& e : kCats) if (e.value == c) return e.text;
    return "?";
}
inline QString colText(style::Color c) {
    for (const auto& e : kCols) if (e.value == c) return e.text;
    return "?";
}
inline QRgb colRgb(style::Color c) {
    for (const auto& e : kCols) if (e.value == c) return e.rgb;
    return 0xB0A89C;
}

// Fotoğraf varsa onu, yoksa ürünün renginde adı yazılı yer tutucuyu döndürür.
inline QPixmap productPixmap(const style::Clothing& c, const QString& sourceDir,
                             int w, int h) {
    QPixmap photo(sourceDir + "/" + QString::fromStdString(c.imagePath));
    if (!photo.isNull()) return photo.scaled(w, h, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    QPixmap pm(w, h);
    const QColor bg(colRgb(c.color));
    pm.fill(bg);
    QPainter p(&pm);
    p.setPen(bg.lightness() < 128 ? QColor("#F5F0E8") : QColor("#3B2A20"));
    p.drawText(QRect(4, 4, w - 8, h - 8), Qt::AlignCenter | Qt::TextWordWrap,
               w >= 100 ? QString::fromStdString(c.name) : QString());
    return pm;
}

}  // namespace gui