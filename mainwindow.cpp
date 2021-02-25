#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "qdebug.h"
#include "QFileDialog"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    m_Scene = new Graphics::WGraphicsScene();
    m_widget = new Graphics::WGraphicsWidget(u8"test");
    m_widget->setScene(m_Scene);
    ui->ver_layout->addWidget(m_widget);
    m_widget->updatePos();
    time = new QTime();

    connect(&timer, &QTimer::timeout, [this]() {
        if (index >= imgs.size()) index = 0;
        m_widget->setWidgetName(dir->entryList()[index]);
        m_Scene->setCenterImage(imgs[index++]);
    });
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
