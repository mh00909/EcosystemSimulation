#include <iostream>
#include <QApplication>
#include "Reserve.h"
#include "../gui/MainWindow.h"

int main(int argc, char *argv[]) {

    QApplication app(argc, argv);

    MainWindow window;
    window.show();

    return app.exec();
}
