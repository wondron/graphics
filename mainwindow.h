#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "wgraphicswidget.h"
#include "wgraphicsscene.h"
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
    void on_spinBox_valueChanged(int arg1);

private:
    Ui::MainWindow *ui;
    Graphics::WGraphicsScene *m_Scene;
    Graphics::WGraphicsWidget *m_widget;
    QTimer timer;
    QDir* dir;
    QVector<QImage> imgs;
    int index = 0;
};
#endif // MAINWINDOW_H
