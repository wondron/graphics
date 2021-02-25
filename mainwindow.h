﻿#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "wimageitem.h"
#include "wgraphicswight.h"
#include "QGraphicsScene"
#include "QTimer"
#include "qdir.h"
#include "QTime"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_btn_image_clicked();

private:
    Ui::MainWindow *ui;
    Graphics::WImageItem *imageItem;
    QGraphicsScene *m_Scene;
    WGraphicsWiget *m_widget;
    QTimer timer;
    QTime* time;
    QDir* dir;
    QVector<QImage> imgs;
    int index = 0;
};
#endif // MAINWINDOW_H
