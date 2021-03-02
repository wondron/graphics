#include "aiwidget.h"
#include "ui_aiwidget.h"
#include "wgraphicswidget.h"
#include "QDebug"
#include "qfiledialog.h"
#include "QMessageBox"
#include "QtConcurrent"

#define CHECKERR(ifstr, errStr)\
if (ifstr){\
emit d->objSignal->sendErr(errStr);\
return;\
}

using namespace Graphics;

class AiWidetPrivate
{
public:
    AiWidetPrivate(AiWidget *pp): p(pp) {}
    ~AiWidetPrivate()
    {
        delete srcWidget;
        delete labelWidget;
        delete objSignal;
        delete fileLabel;
    }
    AiWidget *p;
    QLabel *fileLabel = new QLabel();
    CSignalObj *objSignal = new CSignalObj();
    WGraphicsWidget *srcWidget = new WGraphicsWidget();
    WGraphicsWidget *labelWidget = new WGraphicsWidget();
};


AiWidget::AiWidget(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::AiWidget), d(new AiWidetPrivate(this))
{
    ui->setupUi(this);
    iniLayout();
    iniIcon();

    //信号对象的连接
    connect(d->objSignal, &CSignalObj::sendErr, [this](QString a) {
        QMessageBox::question(this, u8"提示", a, QMessageBox::Ok);
    });

    connect(d->srcWidget->view(), &Graphics::WGraphicsView::dragChanged, [ = ]() {
        QPointF center = d->srcWidget->view()->mapToScene(d->srcWidget->view()->rect().center().x(), d->srcWidget->view()->rect().center().y());
        d->labelWidget->view()->centerOn(center.x(), center.y());
    });

    connect(d->labelWidget->view(), &Graphics::WGraphicsView::dragChanged, [ = ]() {
        QPointF center = d->labelWidget->view()->mapToScene(d->labelWidget->view()->rect().center().x(), d->labelWidget->view()->rect().center().y());
        d->srcWidget->view()->centerOn(center.x(), center.y());
    });

    connect(d->labelWidget->view(), &Graphics::WGraphicsView::scaleChanged, d->srcWidget->view(), &Graphics::WGraphicsView::zoom);
    connect(d->srcWidget->view(), &Graphics::WGraphicsView::scaleChanged, d->labelWidget->view(), &Graphics::WGraphicsView::zoom);

    connect(ui->btn_open, &QPushButton::clicked, this, &AiWidget::readImageTool);
}

AiWidget::~AiWidget()
{
    delete ui;
}

void AiWidget::iniIcon()
{
    QString str = ":/aiImageIcon/aiImgIcon/back.png";
    ui->btn_back->setIconSize(QSize(ICONSIZE * 2, ICONSIZE / 2));
    ui->btn_back->setIcon(QIcon(QPixmap(str)));

    str = ":/aiImageIcon/aiImgIcon/next.png";
    ui->btn_next->setIconSize(QSize(ICONSIZE * 2, ICONSIZE / 2));
    ui->btn_next->setIcon(QIcon(QPixmap(str)));

    QStringList list;
    list << "open" << "save" << "saveOne"
         << "pen" << "polygon" << "delete";

    QList<QPushButton *> btnList;
    btnList << ui->btn_open << ui->btn_save << ui->btn_saveOne
            << ui->btn_pen << ui->btn_polygon << ui->btn_delete;

    for (int i = 0 ; i < list.size() ; i ++) {
        str = QString(":/aiImageIcon/aiImgIcon/%1.png").arg(list[i]);
        btnList[i]->setIcon(QIcon(QPixmap(str)));
        btnList[i]->setIconSize(QSize(ICONSIZE, ICONSIZE));
    }
}

void AiWidget::iniLayout()
{
    ui->hlay_src->addWidget(d->srcWidget);
    ui->hlay_label->addWidget(d->labelWidget);
    ui->statusbar->addWidget(d->labelWidget);
}

void AiWidget::readImageTool()
{
    QString directory = QFileDialog::getExistingDirectory(this, u8"选择图片文件夹");
    CHECKERR(directory == "", u8"所选文件夹为空");

    QFileInfoList list;
    d->fileLabel->setText(u8"当前操作文件夹：" + directory);

    int listsize = ui->listWidget->size().width() - 40;

    QtConcurrent::run( [=](){

    } );
}
