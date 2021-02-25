#ifndef WGRAPHICSWIGHT_H
#define WGRAPHICSWIGHT_H

#include <QFrame>
#include <QGraphicsView>
#include "wgraphicsview.h"
#include "QTimer"

QT_BEGIN_NAMESPACE
class QLabel;
class QToolButton;
class QSlider;
QT_END_NAMESPACE

class WGraphicsWiget : public QFrame
{
    Q_OBJECT
public:
    enum{FITBTN, ZOOMIN, ZOOMOUT, CENTERON, ORIGIN, PRINTER, OPENGL, ANTIALIASE};
    explicit WGraphicsWiget(const QString &name = "", QWidget *parent = nullptr);
    WGraphicsView *view() const;
    inline QString widgetName() {return m_name;}

private slots:
    void resetView();
    void setResetButtonEnabled();
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
    void initNameLabel();
    void initFpsLabel();

private:
    WGraphicsView *graphicsView;
    QString m_name;
    QWidget *m_panel;
    QList<QString> m_btnsObjName, m_tipName;
    QColor flowBgColor;
    QColor flowPressColor;

    QLabel *m_scaleLabel;
    QLabel *m_aimLabel;
    QLabel *m_nameLabel;
    QLabel *m_fpsLabel;
    QTimer *timer;
};

#endif // WGRAPHICSWIGHT_H
