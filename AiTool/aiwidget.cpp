#include "aiwidget.h"
#include "ui_aiwidget.h"
#include "wgraphicswidget.h"
#include "QDebug"

AiWidget::AiWidget(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::AiWidget)
{
    ui->setupUi(this);

    Graphics::WGraphicsWidget *srcWidget = new Graphics::WGraphicsWidget();
    Graphics::WGraphicsWidget *labelWidget = new Graphics::WGraphicsWidget();
    ui->hlay_src->addWidget(srcWidget);
    ui->hlay_label->addWidget(labelWidget);

    connect(srcWidget->view(), &Graphics::WGraphicsView::dragChanged, [ = ]() {
        QPointF center = srcWidget->view()->mapToScene(srcWidget->view()->rect().center().x(), srcWidget->view()->rect().center().y());
        labelWidget->view()->centerOn(center.x() - 1.8, center.y() - 1.8);
    });

    connect(labelWidget->view(), &Graphics::WGraphicsView::dragChanged, [ = ]() {
        QPointF center = labelWidget->view()->mapToScene(labelWidget->view()->rect().center().x(), labelWidget->view()->rect().center().y());
        srcWidget->view()->centerOn(center.x() - 1.8, center.y() - 1.8);
    });

    connect(labelWidget->view(), &Graphics::WGraphicsView::scaleChanged, srcWidget->view(), &Graphics::WGraphicsView::zoom);
    connect(srcWidget->view(), &Graphics::WGraphicsView::scaleChanged, labelWidget->view(), &Graphics::WGraphicsView::zoom);

    QString str = ":/aiImageIcon/aiImgIcon/open.png";
    ui->btn_back->setText("hehe");
    ui->btn_back->setIconSize(QSize(12, 12));
    ui->btn_back->setIcon(QIcon(QPixmap(str)));
}

AiWidget::~AiWidget()
{
    delete ui;
}
