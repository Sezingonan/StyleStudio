#pragma once
#include <QMainWindow>
#include <QPixmap>
#include <QString>

#include "managers/ClothingManager.h"
#include "managers/OutfitEditor.h"
#include "managers/OutfitManager.h"

class QComboBox;
class QDoubleSpinBox;
class QGridLayout;
class QLabel;
class QLineEdit;
class QStringListModel;
class QWidget;
class OutfitPanel;

// Katalog ekranı: arama, filtre, sıralama ve ürün kartları ızgarası.
class CatalogWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit CatalogWindow(const QString& sourceDir, QWidget* parent = nullptr);

private slots:
    void refresh();                              // filtreleri uygula, kartları yeniden çiz
    void updateSuggestions(const QString& text);  // Trie önerileri

private:
    QWidget* makeCard(const style::Clothing& c);
    QPixmap cardPixmap(const style::Clothing& c) const;

    QString sourceDir_;
    style::ClothingManager items_;
    style::OutfitEditor editor_;  // üzerinde çalışılan kombin
    style::OutfitManager outfits_;  // kayıtlı kombinler
    OutfitPanel* panel_ = nullptr;

    QLineEdit* search_ = nullptr;
    QComboBox* category_ = nullptr;
    QComboBox* color_ = nullptr;
    QComboBox* sort_ = nullptr;
    QDoubleSpinBox* minPrice_ = nullptr;
    QDoubleSpinBox* maxPrice_ = nullptr;
    QLabel* resultLabel_ = nullptr;
    QWidget* gridHost_ = nullptr;
    QGridLayout* grid_ = nullptr;
    QStringListModel* suggestModel_ = nullptr;
};