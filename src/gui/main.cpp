#include <QApplication>

#include "CatalogWindow.h"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    CatalogWindow window(STYLE_SOURCE_DIR);
    window.show();
    return app.exec();
}