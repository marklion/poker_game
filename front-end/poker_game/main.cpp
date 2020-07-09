#include "initwindow.h"

#include <QApplication>
#include <QFontDatabase>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    InitWindow w;
    w.win_resouce_request();
    QFontDatabase qd;
    if (false == qd.hasFamily("Microsoft YaHei Light"))
    {
        w.show();
    }
    return a.exec();
}
