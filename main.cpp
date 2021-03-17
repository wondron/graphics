#include "mainwindow.h"
#include "autoType/autotypewidget.h"
#include "wgraphicswidget.h"
#include <QApplication>
#include "opencv2/opencv.hpp"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Graphics::WGraphicsWidget* w = new Graphics::WGraphicsWidget();
    w->setFixedSize(600, 400);
    w->show();

    cv::Mat mat;
    return a.exec();
}
