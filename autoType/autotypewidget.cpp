#include "autotypewidget.h"
#include "ui_autotypewidget.h"
#include "wgraphicswidget.h"

class autoTypeWidgetPrivate
{
public:
    autoTypeWidgetPrivate(autoTypeWidget *p) : pp(p)
    {
        m_widget = new Graphics::WGraphicsWidget("autotype", pp);
    }

    ~autoTypeWidgetPrivate()
    {
        delete m_widget;
    }


    autoTypeWidget *pp;
    Graphics::WGraphicsWidget *m_widget;
};


autoTypeWidget::autoTypeWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::autoTypeWidget),
    d(new autoTypeWidgetPrivate(this))
{
    ui->setupUi(this);
    ui->vlayout->addWidget(d->m_widget);

    QImage img = QImage(100,100, QImage::Format_Grayscale8);
    img.fill(200);
    d->m_widget->setImage(img);

    //cv::Mat mat(20, 20, CV_8UC1);
}

autoTypeWidget::~autoTypeWidget()
{
    delete ui;
}
