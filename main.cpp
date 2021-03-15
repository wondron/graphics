#include "mainwindow.h"
#include "autoType/autotypewidget.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    autoTypeWidget* w = new autoTypeWidget();
    w->show();
    return a.exec();
}
