#pragma once
#include <QString>
#include <QWidget>

#include "managers/ClothingManager.h"
#include "managers/OutfitEditor.h"
#include "managers/OutfitManager.h"

class QLabel;
class QLineEdit;
class QListWidget;
class QPushButton;

// Sağdaki kombin panosu: mevcut kombin, Undo/Redo, kaydetme, kayıtlı kombinler.
class OutfitPanel : public QWidget {
    Q_OBJECT
public:
    OutfitPanel(const style::ClothingManager& items, style::OutfitEditor& editor,
                style::OutfitManager& outfits, const QString& sourceDir,
                QWidget* parent = nullptr);

    void refresh();  // editor veya kayıtlı kombinler değişince çağrılır

signals:
    void message(const QString& text);  // durum çubuğu için

private:
    void refreshItems();
    void refreshSaved();
    void saveCurrent();
    void openSelected();
    void deleteSelected();
    void removeSelectedItem();
    QString outfitsPath() const;

    const style::ClothingManager& items_;
    style::OutfitEditor& editor_;
    style::OutfitManager& outfits_;
    QString sourceDir_;

    QListWidget* itemList_ = nullptr;
    QLabel* totalLabel_ = nullptr;
    QPushButton* undoBtn_ = nullptr;
    QPushButton* redoBtn_ = nullptr;
    QLineEdit* nameEdit_ = nullptr;
    QListWidget* savedList_ = nullptr;
};