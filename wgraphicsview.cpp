#include "wgraphicsview.h"
#include "QWheelEvent"
#include "QMouseEvent"
#include "QScrollBar"
#include "QDebug"
#include <QHBoxLayout>
#include "QTime"
#include "QFontDatabase"
#include "QPushButton"

#define ICONSIZE 40

WGraphicsView::WGraphicsView(QWidget *parent) : QGraphicsView(parent)
{
    QTransform matrix;
    setTransform(matrix.scale(1, 1));
    m_timer = new QTimer();
    setViewportUpdateMode(QGraphicsView::SmartViewportUpdate);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    installEventFilter(this);

    initPanel();
    flowBgColor = "#0000a8";
    flowPressColor = "#5EC7D9";
}

void WGraphicsView::wheelEvent(QWheelEvent *e)
{
    if (e->modifiers() & Qt::ControlModifier) {
        if (e->angleDelta().y() > 0)
            zoomIn(1.05);
        else
            zoomOut(0.95);
    } else {
        QGraphicsView::wheelEvent(e);
    }
}

void WGraphicsView::zoomIn(qreal level)
{
    qreal scale = transform().m11();
    scale *= level;
    if (scale < 0.001) return;
    QTransform matrix;
    matrix.scale(scale, scale);
    setTransform(matrix);
}

void WGraphicsView::zoomOut(qreal level)
{
    qreal scale = transform().m11();
    scale *= level;
    if (scale < 0.001) return;
    QTransform matrix;
    matrix.scale(scale, scale);
    setTransform(matrix);
}

void WGraphicsView::fitScene()
{
    if (scene()) {
        auto rw = viewport()->size().width() / scene()->sceneRect().width();
        auto rh = viewport()->size().height() / scene()->sceneRect().height();
        auto sc  = qMin(rw, rh);

        if (qAbs(transform().m11() - sc) < 0.001) return;
        QTransform tr;
        tr.scale(sc, sc);
        setTransform(tr);
        update();
    }
}

void WGraphicsView::mousePressEvent(QMouseEvent *e)
{
    if (e->button() == Qt::MiddleButton) {
        setDragMode(QGraphicsView::ScrollHandDrag);
        //发生中键的按时，返回的还是左键。 第一个left是触发鼠标事件的按键，第二个是触发事件时，鼠标的状态
        QMouseEvent evt(e->type(), e->localPos(), e->windowPos(), e->screenPos(), Qt::LeftButton, Qt::LeftButton, Qt::NoModifier, e->source());
        QGraphicsView::mousePressEvent(&evt);
    }
    QGraphicsView::mousePressEvent(e);
}

void WGraphicsView::mouseReleaseEvent(QMouseEvent *e)
{
    if (e->button() == Qt::MiddleButton) {
        setDragMode(QGraphicsView::NoDrag);
    }
    QGraphicsView::mouseReleaseEvent(e);
}

void WGraphicsView::mouseMoveEvent(QMouseEvent *e)
{
    QGraphicsView::mouseMoveEvent(e);
}

void WGraphicsView::keyPressEvent(QKeyEvent *e)
{
    QGraphicsView::keyPressEvent(e);
}

void WGraphicsView::keyReleaseEvent(QKeyEvent *e)
{
    QGraphicsView::keyReleaseEvent(e);
}

bool WGraphicsView::eventFilter(QObject *watched, QEvent *event)
{
    if (event->type() == QEvent::Paint) {
        PaintOnWidget();
    }
    return QWidget::eventFilter(watched, event);
}

void WGraphicsView::enterEvent(QEvent *e)
{
    m_panel->setVisible(true);
}

void WGraphicsView::leaveEvent(QEvent *)
{
    m_panel->setVisible(false);
}

void WGraphicsView::resizeEvent(QResizeEvent *)
{
    //重新设置顶部工具栏的位置和宽高,可以自行设置顶部显示或者底部显示
    qDebug() << "resize widget";
    m_panel->setGeometry(5, 5, this->width() - ICONSIZE, ICONSIZE);
    //flowPanel->setGeometry(borderWidth, this->height() - height - borderWidth, this->width() - (borderWidth * 2), height);

}

void WGraphicsView::initPanel()
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
    QList<QString> btns;
    btns << u8"适应" << u8"放大" << u8"缩小" << u8"显示基准线";

    //循环添加顶部按钮
    for (int i = 0; i < btns.count(); i++) {
        QPushButton *btn = new QPushButton;
        btn->setIconSize(QSize(40, 40));
        QString str = QString("D:\\icon\\8010\\f%1.ico").arg(i + 1);
        btn->setIcon(QIcon(QPixmap(str)));
        btn->setToolTip(btns[i]);
        //绑定按钮单击事件,用来发出信号通知
        connect(btn, SIGNAL(clicked(bool)), this, SLOT(btnClicked()));

        //设置标识,用来区别按钮
        btn->setObjectName(btns.at(i));

        //设置固定宽度
        btn->setFixedWidth(ICONSIZE + 1);
        btn->setFixedHeight(ICONSIZE + 1);

        //设置拉伸策略使得填充
        btn->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);

        //设置焦点策略为无焦点,避免单击后焦点跑到按钮上
        btn->setFocusPolicy(Qt::NoFocus);

        //将按钮加到布局中
        layout->addWidget(btn);
    }
    initPanelStyle();
}

void WGraphicsView::initPanelStyle()
{
    //设置样式以便区分,可以自行更改样式,也可以不用样式
    QStringList qss;
    QString rgba = QString("rgba(0,0,0,150)");//.arg(flowBgColor.red()).arg(flowBgColor.green()).arg(flowBgColor.blue());
    qss.append(QString("#flowPanel{background:%1;border:none;}").arg(rgba));
    qss.append(QString("QPushButton{border:none;padding:0px;background:rgba(0,0,0,0);}"));
    qss.append(QString("QPushButton:pressed{color:%1;}").arg(flowPressColor.name()));
    m_panel->setStyleSheet(qss.join(""));
}

void WGraphicsView::PaintOnWidget()
{
    QPainter painter2(this);
    QPen pen(QColor(0, 0, 0), 2);
    painter2.setPen(pen);
    QPoint center = viewport()->rect().center();
    painter2.drawEllipse(center.x() - 2, center.y() - 2, 4, 4);
}
