#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "qdebug.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    m_Scene = new QGraphicsScene();
    m_widget = new WGraphicsWiget(u8"test", this);
    m_widget->view()->setScene(m_Scene);
    time = new QTime();

    imageItem = new Graphics::WImageItem();
    m_Scene->addItem(imageItem);

    connect(imageItem, &Graphics::WImageItem::imgSizeDif, m_widget->view(), &WGraphicsView::fitScene);
    connect(imageItem, &Graphics::WImageItem::imgchgeFps, m_widget, &WGraphicsWiget::fpsChange);

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
    dir = new QDir("F:\\img\\mergeimg");
    if (! dir->exists()) {
        qDebug() << "the TDat path is not exit";
        return ;
    }

    dir->setFilter(QDir::NoDotAndDotDot | QDir::AllEntries);
    QStringList temp_folder = dir->entryList();
    if (!temp_folder.size()) return;
    time->start();
    index = 0;

    imgs.clear();
    int a = time->elapsed();
    for (auto i : dir->entryList()) {
        QString str = "F:\\img\\mergeimg\\" + i;
        imgs.push_back(QImage(str));
    }

    timer.start(100);
    connect(&timer, &QTimer::timeout, [this]() {
        if (index >= imgs.size()) index = 0;
        int a = time->elapsed();
        imageItem->setImage(imgs[index]);
        m_widget->setWidgetName(dir->entryList()[index++]);
    });

}
