#include "wcamerwidget.h"
#include "ui_wcamerwidget.h"
#include "wgraphicswidget.h"
#include "wgraphicsscene.h"
#include "QToolButton"

class WCamerWidgetPrivate
{
public:
    WCamerWidgetPrivate(WCamerWidget *q): p(q)
    {
    }

    WCamerWidget *p;
    QString camName;
    Graphics::WGraphicsWidget *widget;
    Graphics::WGraphicsScene  *scene;
};


WCamerWidget::WCamerWidget(QWidget *parent, QString name) :
    QWidget(parent),
    ui(new Ui::WCamerWidget),
    d(new WCamerWidgetPrivate(this))
{
    ui->setupUi(this);
    d->camName = name;
    iniLayout();
    iniSliderStyle(ui->hSlider_exp);
    iniSliderStyle(ui->hSlider_gain);
}

WCamerWidget::~WCamerWidget()
{
    delete ui;
}

void WCamerWidget::iniLayout()
{
    ui->btn_single->setIconSize(QSize(ICONSIZE, ICONSIZE));
    QString str = QString(":/new/icons/image/camera.png");
    ui->btn_single->setIcon(QIcon(QPixmap(str)));

    ui->btn_continue->setIconSize(QSize(ICONSIZE, ICONSIZE));
    str = QString(":/new/icons/image/continue.png");
    ui->btn_continue->setIcon(QIcon(QPixmap(str)));

    ui->btn_saveImg->setIconSize(QSize(ICONSIZE, ICONSIZE));
    str = QString(":/new/icons/image/saveImg.png");
    ui->btn_saveImg->setIcon(QIcon(QPixmap(str)));

    ui->btn_pamset->setIconSize(QSize(ICONSIZE, ICONSIZE));
    str = QString(":/new/icons/image/pamSet.png");
    ui->btn_pamset->setIcon(QIcon(QPixmap(str)));

    d->widget = new Graphics::WGraphicsWidget(d->camName, this);
    d->scene = new Graphics::WGraphicsScene(d->widget);
    d->widget->setCurrentScene(d->scene);
    ui->hlay_Cam->addWidget(d->widget);
}

void WCamerWidget::iniSliderStyle(QSlider *slider)
{
    slider->setStyleSheet( {
        "QSlider::groove:horizontal {"
        "height: 6px;"
        "background: qlineargradient(x1: 0, y1: 1, x2: 1, y2: 0, "
        "stop: 0  rgb(92, 91, 90)"
        "stop: 1.0 rgb(210, 210, 210));"
        "}"

        "QSlider::handle:horizontal {"
        "width: 10px;"
        "background: rgb(80, 130, 220);"
        "margin: -6px 0px -6px 0px;"
        "border-radius: 5px;"
        "}"
    });
}


