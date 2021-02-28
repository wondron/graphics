#ifndef WCAMERWIDGET_H
#define WCAMERWIDGET_H

#include <QWidget>
#include "qslider.h"

namespace Ui {
class WCamerWidget;
}

class WCamerWidgetPrivate;

class WCamerWidget : public QWidget
{
    Q_OBJECT

public:
    enum {ICONSIZE = 20};
    explicit WCamerWidget(QWidget *parent = nullptr, QString name = "");
    ~WCamerWidget();

private:
    void iniLayout();
    void iniSliderStyle(QSlider* slider);
    Ui::WCamerWidget *ui;
    WCamerWidgetPrivate *d;
};

#endif // WCAMERWIDGET_H
