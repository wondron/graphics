﻿#pragma once

#include <QFrame>
#include <QGraphicsView>
#include "wgraphicsview.h"
#include "wgraphicsscene.h"

QT_BEGIN_NAMESPACE
class QLabel;
class QToolButton;
class QSlider;
QT_END_NAMESPACE

namespace Graphics
{
class WGraphicsView;
class WViewPrivate;

class WGraphicsWidget : public QFrame
{
    Q_OBJECT
public:
    enum{FITBTN, ZOOMIN, ZOOMOUT, CENTERON, ORIGIN, PRINTER, OPENGL, ANTIALIASE, LABELSHOW, AIMSWITCH};
    explicit WGraphicsWidget(const QString &name = "", QWidget *parent = nullptr);
    WGraphicsView *view() const;
    QString widgetName() const;
    void setCurrentScene(WGraphicsScene* scene);
    void updatePos();

    /**
     * @brief setLabelShow: 是否显示标注信息。
     */
    void setLabelShow(const bool enabel);
    /**
     * @brief setAimShow: 显示瞄准信息
     */
    void setAimShow(const bool enabel);

    /**
     * @brief setPanelShow: 工具栏显示
     */
    void setPanelShow(const bool enabel);

signals:
    void sendEvent(QEvent* e);

private slots:
    void enterEvent(QEvent *e) override;
    void leaveEvent(QEvent *) override;
    void resizeEvent(QResizeEvent *) override;
    void btnClicked();

public slots:
    void fpsChange(int num);
    void setWidgetName(QString);

private:
    void iniPanel();
    void initPanelStyle();
    void initScaleWidget();
    void initAimWidget();
    //void initNameLabel();
    void initFpsLabel();

private:
    WViewPrivate* d;
};

}
