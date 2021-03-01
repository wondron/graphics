#include "aiwidget.h"
#include "ui_aiwidget.h"
#include "wgraphicswidget.h"
#include "QDebug"

class AiWidetPrivate
{
public:
    AiWidetPrivate(AiWidget *pp): p(pp) {}

    ~AiWidetPrivate()
    {
        delete srcWidget;
        delete labelWidget;
    }
    AiWidget *p;
    Graphics::WGraphicsWidget *srcWidget;
    Graphics::WGraphicsWidget *labelWidget;
};


AiWidget::AiWidget(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::AiWidget), d(new AiWidetPrivate(this))
{
    ui->setupUi(this);

    d->srcWidget = new Graphics::WGraphicsWidget();
    d->labelWidget = new Graphics::WGraphicsWidget();
    ui->hlay_src->addWidget(d->srcWidget);
    ui->hlay_label->addWidget(d->labelWidget);

    iniIcon();

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
