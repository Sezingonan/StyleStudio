#include <QApplication>
#include <QLabel>
#include <QMainWindow>
#include <QString>
#include <string>

#include "managers/ClothingManager.h"
#include "models/ClothingCollection.h"
#include "storage/JsonManager.h"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    // Çekirdek kütüphane Qt'siz: aynı yükleme kodu burada da çalışır.
    const std::string path = std::string(STYLE_SOURCE_DIR) + "/data/clothes.json";
    style::ClothingCollection collection;
    style::LoadReport report = style::JsonManager::loadClothes(path, collection);
    style::ClothingManager manager;
    if (report.ok()) manager.loadFrom(collection);

    QString text = report.ok()
        ? QString("Style Studio\n\n%1 ürün yüklendi").arg(manager.size())
        : QString("Veri yüklenemedi:\n%1").arg(QString::fromStdString(report.fatalError));

    QLabel* label = new QLabel(text);
    label->setAlignment(Qt::AlignCenter);
    // QSS: CSS'e çok benzer. Renk paleti: kırık beyaz zemin, koyu kahve yazı.
    label->setStyleSheet("background-color: #F5F0E8; color: #3B2A20; font-size: 26px;");

    QMainWindow window;
    window.setWindowTitle("Style Studio");
    window.setCentralWidget(label);
    window.resize(900, 600);
    window.show();
    return app.exec();
}