#include <QApplication>

#include "MainWindow.h"

int main(int argc, char *argv[]) {
    QApplication application(argc, argv);

    MainWindow w;
    w.resize(800, 600);
    w.show();

    return application.exec();
}