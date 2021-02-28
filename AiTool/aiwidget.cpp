#include "aiwidget.h"
#include "ui_aiwidget.h"
#include "wgraphicswidget.h"
#include "QDebug"

class AiWidetPrivate
{
public:
    AiWidetPrivate(AiWidget *pp): p(pp){}

    ~AiWidetPrivate(){
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

    QString str = ":/aiImageIcon/aiImgIcon/open.png";
    ui->btn_back->setText("hehe");
    ui->btn_back->setIconSize(QSize(12, 12));
    ui->btn_back->setIcon(QIcon(QPixmap(str)));
}

AiWidget::~AiWidget()
{
    delete ui;
}
