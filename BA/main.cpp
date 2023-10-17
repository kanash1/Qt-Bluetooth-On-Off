#include "BluetoothWindow.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    BluetoothWindow w;
    w.show();
    return a.exec();
}
