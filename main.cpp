#include "photoprint.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    PhotoPrint w;
    w.show();
    return a.exec();
}
