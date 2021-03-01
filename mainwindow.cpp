#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "qdebug.h"
#include "QFileDialog"
#include "wcircleitem.h"
#include "camTool/wcamerwidget.h"
#include "AiTool/aiwidget.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    AiWidget *widget = new AiWidget();
    //Graphics::WGraphicsWidget* widget = new Graphics::WGraphicsWidget();
    widget->setWindowFlag(Qt::Dialog);
    widget->show();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_btn_image_clicked()
{
    timer.stop();
    QString fileName = QFileDialog::getExistingDirectory(this);
    if (fileName.isEmpty()) return;
    dir = new QDir(fileName);
    if (! dir->exists()) {
        qDebug() << "the TDat path is not exit";
        return ;
    }

    dir->setFilter(QDir::NoDotAndDotDot | QDir::AllEntries);
    QStringList temp_folder = dir->entryList();
    if (!temp_folder.size()) return;
    index = 0;

    imgs.clear();
    for (auto i : dir->entryList()) {
        QString str = fileName + "\\" + i;
        QImage img(str);
        if(img.isNull()) continue;
        imgs.push_back(img);
    }
    timer.start(10);
}

void MainWindow::on_spinBox_valueChanged(int arg1)
{
    timer.stop();
    timer.start(arg1);
}
