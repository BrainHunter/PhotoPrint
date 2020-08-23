#include "photoprint.h"

#include <QApplication>

int main(int argc, char *argv[])
{

    #ifdef _WIN32
    if (AttachConsole(ATTACH_PARENT_PROCESS)) {
        freopen("CONOUT$", "w", stdout);
        freopen("CONOUT$", "w", stderr);
    }
    #endif

    QApplication a(argc, argv);
    PhotoPrint w;
    w.show();
    return a.exec();
}
