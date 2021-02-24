#pragma execution_character_set("utf-8")

#include "frmvideowidget.h"
#include "ui_frmvideowidget.h"

frmVideoWidget::frmVideoWidget(QWidget *parent) : QWidget(parent), ui(new Ui::frmVideoWidget)
{
    ui->setupUi(this);
    this->initForm();
}

frmVideoWidget::~frmVideoWidget()
{
    delete ui;
}

void frmVideoWidget::initForm()
{
    ui->videoWidget1->setFlowEnable(true);
    ui->videoWidget1->setBgText("hehe");

    //ui->videoWidget1->setBgImage(QImage("D:\\icon\\test.jpg"));
    ui->videoWidget1->setOSD1Image(QImage("D:\\icon\\8010\\f1.ico"));
    ui->videoWidget1->setOSD1Visible(true);
    ui->videoWidget1->updateImage(QImage("D:\\icon\\test.jpg"));
    connect(ui->videoWidget1, SIGNAL(btnClicked(QString)), this, SLOT(btnClicked(QString)));
}

void frmVideoWidget::btnClicked(const QString &objName)
{
    VideoWidget *videoWidget = (VideoWidget *)sender();
    QString str = QString("当前单击了控件 %1 的按钮 %2").arg(videoWidget->objectName()).arg(objName);
    ui->label->setText(str);
}
