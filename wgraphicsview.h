#ifndef WGRAPHICSVIEW_H
#define WGRAPHICSVIEW_H

#include <QGraphicsView>
#include "QTimer"
#include "QLabel"

class WGraphicsView : public QGraphicsView
{
    Q_OBJECT
public:
    WGraphicsView(QWidget *parent = nullptr);
    void zoomIn(qreal level = 1.05);
    void zoomOut(qreal level = 0.95);
    void fitScene();

protected:
    void wheelEvent(QWheelEvent *) override;
    void mousePressEvent(QMouseEvent *e) override;
    void mouseReleaseEvent(QMouseEvent *e) override;
    void mouseMoveEvent(QMouseEvent *e) override;
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    bool eventFilter(QObject *watched, QEvent *event) override;
    //void changeEvent(QEvent *event) override;
    void enterEvent(QEvent *e) override;
    void leaveEvent(QEvent *) override;
    void resizeEvent(QResizeEvent *) override;

private:
    void initPanel();
    void initScaleWidget();
    void initPanelStyle();
    void initAimWidget();

signals:
    void scaleChanged(qreal);

public slots:
    void updateScaleInfo(qreal);

private:
    QTimer *m_timer;
    int m_scale;
    QWidget *m_panel;
    QLabel *m_scaleLabel;
    QLabel *m_aimLabel;
    QColor flowBgColor;
    QColor flowPressColor;
};

#endif // WGRAPHICSVIEW_H
