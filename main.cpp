
#include "datmessage.h"
#include "mainwindow.h"

#include <QApplication>

int main(int argc, char **argv) {

    QApplication app(argc, argv);

    qRegisterMetaType<DatMessage>();

    MainWindow w;
    w.show();
    return app.exec();
}
