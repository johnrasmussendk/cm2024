
#include "datmessage.h"
#include "supmessage.h"
#include "mainwindow.h"

#include <QApplication>

int main(int argc, char **argv) {

    QApplication app(argc, argv);

    qRegisterMetaType<DatMessage>();
    qRegisterMetaType<SupMessage>();

    MainWindow w;
    w.show();
    return app.exec();
}
