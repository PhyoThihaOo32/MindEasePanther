#include "app/mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    app.setApplicationName("MindEase");
    app.setOrganizationName("MindEase");

    MainWindow w;
    w.show();
    return app.exec();
}
