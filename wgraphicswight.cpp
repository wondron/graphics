#include "wgraphicswight.h"

#if defined(QT_PRINTSUPPORT_LIB)
#include <QtPrintSupport/qtprintsupportglobal.h>
#if QT_CONFIG(printdialog)
#include <QPrinter>
#include <QPrintDialog>
#endif
#endif
#ifndef QT_NO_OPENGL
#include <QtOpenGL>
#else
#include <QtWidgets>
#endif
#include <QtMath>

#define ICONSIZE 30

WGraphicsWiget::WGraphicsWiget(const QString &name, QWidget *parent)
    : QFrame(parent), m_name(name)
{
    setFrameStyle(Sunken | StyledPanel);
    graphicsView = new WGraphicsView(this);
    timer = new QTimer();
    int size = style()->pixelMetric(QStyle::PM_ToolBarIconSize);
    QSize iconSize(size, size);

    m_btnsObjName << u8"fitBtn" << u8"zoomIn" << "zoomOut"
                  << u8"centerOn" << u8"origin" << "printer"
                  << "OpenGL" << "Antialiasing";
    m_tipName   << u8"适应" << u8"放大" << u8"缩小"
                << u8"定位中心" << u8"原始尺寸"
                << u8"打印" << u8"OpenGL开关"
                << u8"反锯齿开关";

    flowBgColor = QColor(150, 150, 150);
    flowPressColor = QColor( 255, 0, 0);
    iniPanel();
    initAimWidget();
    initScaleWidget();
    initNameLabel();
    initFpsLabel();

    connect(graphicsView, &WGraphicsView::scaleChanged, [this](qreal scale) {
        QString str = QString::number(scale * 100, 'f', 1);
        m_scaleLabel->setText(str + "%");
    });

#ifndef QT_NO_OPENGL
    //openGlButton->setEnabled(QGLFormat::hasOpenGL());
#else
    //openGlButton->setEnabled(false);
#endif
}

WGraphicsView *WGraphicsWiget::view() const
{
    return graphicsView;
}

void WGraphicsWiget::setWidgetName(QString name)
{
    m_name = name;
    m_nameLabel->setFixedSize(m_name.size() * 8, 25);
    m_nameLabel->setText(name);
}

void WGraphicsWiget::resetView()
{
    graphicsView->ensureVisible(QRectF(0, 0, 0, 0));
    //resetButton->setEnabled(false);
}

void WGraphicsWiget::setResetButtonEnabled()
{
    //resetButton->setEnabled(true);
}

void WGraphicsWiget::enterEvent(QEvent *e)
{
    Q_UNUSED(e);
    m_panel->setVisible(true);
}

void WGraphicsWiget::leaveEvent(QEvent *e)
{
    Q_UNUSED(e);
    m_panel->setVisible(false);
}

void WGraphicsWiget::resizeEvent(QResizeEvent *)
{
    m_panel->setGeometry(5, 5, this->width() - ICONSIZE, ICONSIZE * 1.4);
    if (!view()) return;

    graphicsView->setFixedSize(this->width(), this->height());

    m_aimLabel->setGeometry(graphicsView->viewport()->rect().center().x() - m_aimLabel->width() / 2,
                            graphicsView->viewport()->rect().center().y() - m_aimLabel->height() / 2,
                            m_aimLabel->width(),
                            m_aimLabel->height());

    m_scaleLabel->setGeometry(graphicsView->viewport()->rect().width() - m_scaleLabel->width() - 5,
                              graphicsView->viewport()->rect().height() - m_scaleLabel->height(),
                              m_scaleLabel->width(), m_scaleLabel->height());

    m_nameLabel->setGeometry(5, 5, m_nameLabel->width(), m_nameLabel->height());
    m_fpsLabel->setGeometry(5, 30, m_fpsLabel->width(), m_fpsLabel->height());
}

void WGraphicsWiget::btnClicked()
{
    if (!view()) return;

    QPushButton *btn = (QPushButton *)sender();
    QString objName = btn->objectName();

    int clickNum;
    for (int i = 0 ; i < m_btnsObjName.size() ; i++) {
        if (objName == m_btnsObjName[i])
            clickNum = i;
    }

    switch (clickNum) {
    case FITBTN :
        graphicsView->fitScene();
        break;
    case ZOOMIN :
        graphicsView->zoomIn();
        break;
    case ZOOMOUT :
        graphicsView->zoomOut();
        break;
    case CENTERON :
        graphicsView->movetoCenter();
        break;
    case ORIGIN :
        graphicsView->zoom(1);
        break;
    case PRINTER :
        graphicsView->print();
        break;
    case OPENGL  :
        graphicsView->setOpenGl(btn->isChecked());
        break;
    case ANTIALIASE:
        graphicsView->setAntialiase(btn->isChecked());
        break;
    }
}

void WGraphicsWiget::fpsChange(int num)
{
    QString str = QString("fps: %1").arg(num);
    m_fpsLabel->setFixedSize(str.size() * 10, 25);
    m_fpsLabel->setText(str);
}

void WGraphicsWiget::iniPanel()
{
    m_panel = new QWidget(this);
    m_panel->setObjectName("flowPanel");

    //用布局顶住,左侧弹簧
    QHBoxLayout *layout = new QHBoxLayout;
    layout->setSpacing(2);
    layout->setMargin(0);
    layout->addStretch();
    m_panel->setLayout(layout);
    m_panel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    //按钮集合名称,如果需要新增按钮则在这里增加即可
    QList<QString> imgName;
    imgName << "Expand" << "Zoom In" << "Zoom Out"
            << "Center Direction" << "Original Size"
            << "printer" << "OpenGL" << "Antialiasing";

    //循环添加顶部按钮
    for (int i = 0; i < m_btnsObjName.count(); i++) {
        QPushButton *btn = new QPushButton;
        btn->setIconSize(QSize(ICONSIZE, ICONSIZE));

        //绑定按钮单击事件,用来发出信号通知
        connect(btn, SIGNAL(clicked(bool)), this, SLOT(btnClicked()));

        QString str = QString(":/new/icons/image/%1.png").arg(imgName[i]);
        btn->setIcon(QIcon(QPixmap(str)));
        btn->setToolTip(m_tipName[i]);

        //设置标识,用来区别按钮
        btn->setObjectName(m_btnsObjName.at(i));

        if (imgName[i] == "OpenGL" || imgName[i] == "Antialiasing" )
            btn->setCheckable(true);

        //设置固定宽度
        btn->setFixedWidth(ICONSIZE + 8);
        btn->setFixedHeight(ICONSIZE + 8);

        //设置拉伸策略使得填充
        btn->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);

        //设置焦点策略为无焦点,避免单击后焦点跑到按钮上
        btn->setFocusPolicy(Qt::NoFocus);

        //将按钮加到布局中
        layout->addWidget(btn);
    }
    initPanelStyle();
}

void WGraphicsWiget::initPanelStyle()
{
    //设置样式以便区分,可以自行更改样式,也可以不用样式
    QStringList qss;
    QString rgba = QString("rgba(%1, %2, %3, 170)").arg(flowBgColor.red()).arg(flowBgColor.green()).arg(flowBgColor.blue());
    //qss.append(QString("#flowPanel{background:%1;}").arg(rgba));
    qss.append(QString("QPushButton{color:rgba(0,0,0,0);}"));
    qss.append(QString("QPushButton:pressed{color:rgb(255,0,0);}"));
    //qss.append(QString("QPushButton{border:none;padding:0px;background:rgba(0,0,0,0);}"));
    //qss.append(QString("QPushButton:pressed{color:%1;}").arg(flowPressColor.name()));
    qDebug() << "flowPressColor:" << flowPressColor << "rgba:" << flowBgColor.red();
    m_panel->setStyleSheet(qss.join(""));
}

void WGraphicsWiget::initScaleWidget()
{
    m_scaleLabel = new QLabel("100%", this);
    QFont font("Arial");
    font.setBold(true);
    font.setPixelSize(12);
    m_scaleLabel->setFixedSize(50, 25);
    m_scaleLabel->setFont(font);
    m_scaleLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    m_scaleLabel->setStyleSheet("QLabel{background-color:rgb(100,100,100);color:rgb(0,200,0);border: 2px solid rgb(100,200,100); border-radius: 5px;}");
}

void WGraphicsWiget::initAimWidget()
{
    m_aimLabel = new QLabel(this);
    QImage img(":/new/icons/image/aims.png");
    m_aimLabel->setFixedSize(img.width(), img.height());
    m_aimLabel->setPixmap(QPixmap::fromImage(img));
}

void WGraphicsWiget::initNameLabel()
{
    m_nameLabel = new QLabel(this);
    m_nameLabel->setText(m_name);
    QFont font("Arial");
    font.setBold(true);
    font.setPixelSize(13);
    m_nameLabel->setFixedSize(m_name.size() * 8, 25);
    m_nameLabel->setFont(font);
    m_nameLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    m_nameLabel->setStyleSheet("color:rgb(0,200,0);");
}

void WGraphicsWiget::initFpsLabel()
{
    m_fpsLabel = new QLabel(this);
    m_fpsLabel->setText(m_name);
    QFont font("Arial");
    font.setBold(true);
    font.setPixelSize(13);
    m_fpsLabel->setFixedSize(m_name.size() * 10, 25);
    m_fpsLabel->setFont(font);
    m_fpsLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    m_fpsLabel->setText("fps: 0");
    m_fpsLabel->setStyleSheet("color:rgb(0,200,0);");
}
