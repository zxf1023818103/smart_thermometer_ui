#include "thermometerwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ThermometerWindow w;
    w.show();
    return a.exec();
}
