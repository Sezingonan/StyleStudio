#include "OutfitPanel.h"

#include <QHBoxLayout>
#include <QIcon>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QPushButton>
#include <QVBoxLayout>

#include "GuiCommon.h"
#include "storage/OutfitStorage.h"

using namespace style;

OutfitPanel::OutfitPanel(const ClothingManager& items, OutfitEditor& editor,
                         OutfitManager& outfits, const QString& sourceDir,
                         QWidget* parent)
    : QWidget(parent), items_(items), editor_(editor), outfits_(outfits),
      sourceDir_(sourceDir) {
    setMinimumWidth(320);
    QVBoxLayout* root = new QVBoxLayout(this);

    QLabel* title = new QLabel("Kombinim");
    title->setStyleSheet("font-size: 20px; font-weight: 600;");
    root->addWidget(title);

    itemList_ = new QListWidget;
    itemList_->setIconSize(QSize(48, 60));
    root->addWidget(itemList_, 2);

    totalLabel_ = new QLabel;
    totalLabel_->setStyleSheet("font-weight: 600;");
    root->addWidget(totalLabel_);

    // Çıkar / Geri al / İleri al
    QHBoxLayout* actions = new QHBoxLayout;
    QPushButton* removeBtn = new QPushButton("Çıkar");
    undoBtn_ = new QPushButton("Geri al");
    redoBtn_ = new QPushButton("İleri al");
    QPushButton* clearBtn = new QPushButton("Temizle");
    actions->addWidget(removeBtn);
    actions->addWidget(undoBtn_);
    actions->addWidget(redoBtn_);
    actions->addWidget(clearBtn);
    root->addLayout(actions);

    // Kaydet
    nameEdit_ = new QLineEdit;
    nameEdit_->setPlaceholderText("Kombin adı");
    root->addWidget(nameEdit_);
    QPushButton* saveBtn = new QPushButton("Kombini kaydet");
    root->addWidget(saveBtn);

    // Kayıtlı kombinler
    QLabel* savedTitle = new QLabel("Kayıtlı kombinler");
    savedTitle->setStyleSheet("font-size: 16px; font-weight: 600; margin-top: 10px;");
    root->addWidget(savedTitle);
    savedList_ = new QListWidget;
    root->addWidget(savedList_, 1);
    QHBoxLayout* savedActions = new QHBoxLayout;
    QPushButton* openBtn = new QPushButton("Aç");
    QPushButton* deleteBtn = new QPushButton("Sil");
    savedActions->addWidget(openBtn);
    savedActions->addWidget(deleteBtn);
    root->addLayout(savedActions);

    connect(removeBtn, &QPushButton::clicked, this, [this] { removeSelectedItem(); });
    connect(undoBtn_, &QPushButton::clicked, this, [this] {
        if (editor_.undo()) refresh();
    });
    connect(redoBtn_, &QPushButton::clicked, this, [this] {
        if (editor_.redo()) refresh();
    });
    connect(clearBtn, &QPushButton::clicked, this, [this] {
        if (editor_.clear()) {
            refresh();
            emit message("Kombin temizlendi (geri alınabilir)");
        }
    });
    connect(saveBtn, &QPushButton::clicked, this, [this] { saveCurrent(); });
    connect(openBtn, &QPushButton::clicked, this, [this] { openSelected(); });
    connect(deleteBtn, &QPushButton::clicked, this, [this] { deleteSelected(); });
    connect(savedList_, &QListWidget::itemDoubleClicked, this, [this] { openSelected(); });

    refresh();
}

QString OutfitPanel::outfitsPath() const { return sourceDir_ + "/data/outfits.json"; }

void OutfitPanel::refresh() {
    refreshItems();
    refreshSaved();
}

void OutfitPanel::refreshItems() {
    itemList_->clear();
    double total = 0;
    for (int id : editor_.items()) {
        const Clothing* c = items_.findById(id);
        QListWidgetItem* row = new QListWidgetItem;
        if (c) {
            row->setText(QString::fromStdString(c->name) + "\n" +
                         QString::number(c->price, 'f', 0) + " TL");
            row->setIcon(QIcon(gui::productPixmap(*c, sourceDir_, 48, 60)));
            total += c->price;
        } else {
            row->setText(QString("(silinmiş ürün #%1)").arg(id));
        }
        row->setData(Qt::UserRole, id);
        itemList_->addItem(row);
    }
    totalLabel_->setText(editor_.empty()
        ? QString("Kombin boş. Katalogdan ürün ekle.")
        : QString("%1 ürün, toplam %2 TL").arg(editor_.size()).arg(total, 0, 'f', 0));
    undoBtn_->setEnabled(editor_.canUndo());
    redoBtn_->setEnabled(editor_.canRedo());
}

void OutfitPanel::refreshSaved() {
    savedList_->clear();
    for (const Outfit& o : outfits_.all()) {
        QString text = QString("#%1  %2  (%3 ürün, %4 TL")
                           .arg(o.outfitId)
                           .arg(QString::fromStdString(o.name))
                           .arg(o.clothingIds.size())
                           .arg(OutfitManager::totalPrice(o, items_), 0, 'f', 0);
        const std::size_t missing = OutfitManager::missingCount(o, items_);
        if (missing > 0) text += QString(", %1 eksik").arg(missing);
        text += ")";
        QListWidgetItem* row = new QListWidgetItem(text);
        row->setData(Qt::UserRole, o.outfitId);
        savedList_->addItem(row);
    }
}

void OutfitPanel::removeSelectedItem() {
    QListWidgetItem* row = itemList_->currentItem();
    if (!row) {
        emit message("Çıkarmak için kombindeki bir ürünü seç");
        return;
    }
    if (editor_.remove(row->data(Qt::UserRole).toInt())) refresh();
}

void OutfitPanel::saveCurrent() {
    const QString name = nameEdit_->text().trimmed();
    const int id = outfits_.create(name.toStdString(), editor_.items());
    if (id == 0) {
        emit message("Kaydedilemedi: bir ad yaz ve en az bir ürün ekle");
        return;
    }
    const bool written = OutfitStorage::save(outfitsPath().toStdString(), outfits_);
    emit message(written ? QString("Kombin #%1 kaydedildi").arg(id)
                         : QString("Kombin bellekte tutuldu ama dosyaya yazılamadı"));
    nameEdit_->clear();
    refreshSaved();
}

void OutfitPanel::openSelected() {
    QListWidgetItem* row = savedList_->currentItem();
    if (!row) {
        emit message("Açmak için kayıtlı bir kombin seç");
        return;
    }
    const Outfit* o = outfits_.find(row->data(Qt::UserRole).toInt());
    if (!o) return;
    editor_.clear();  // geri alınabilir
    for (int id : o->clothingIds) {
        if (items_.findById(id)) editor_.add(id);  // silinmiş ürünler atlanır
    }
    nameEdit_->setText(QString::fromStdString(o->name));
    refresh();
    emit message(QString("\"%1\" açıldı").arg(QString::fromStdString(o->name)));
}

void OutfitPanel::deleteSelected() {
    QListWidgetItem* row = savedList_->currentItem();
    if (!row) {
        emit message("Silmek için kayıtlı bir kombin seç");
        return;
    }
    if (outfits_.remove(row->data(Qt::UserRole).toInt())) {
        OutfitStorage::save(outfitsPath().toStdString(), outfits_);
        refreshSaved();
        emit message("Kombin silindi");
    }
}