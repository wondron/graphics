#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    m_Scene = new QGraphicsScene();
    m_widget = new WGraphicsWiget(u8"hehe", this);
    m_widget->view()->setScene(m_Scene);

    imageItem = new Graphics::WImageItem();
    m_Scene->addItem(imageItem);

    m_widget->setWindowFlag(Qt::Dialog);
    m_widget->show();

    m_widget->setMinimumSize(800, 600);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_btn_image_clicked()
{
    //QImage img("F:/img/0-3-4.bmp");
    QImage img(R"(F:\img\0-3-4.bmp)");
    imageItem->setImage(img);
}
