#include "mainprocess.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainProcess w;
    w.show();

    return a.exec();
}
