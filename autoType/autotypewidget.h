#ifndef AUTOTYPEWIDGET_H
#define AUTOTYPEWIDGET_H

#include <QWidget>

namespace Ui {
class autoTypeWidget;
}

class autoTypeWidgetPrivate;

class autoTypeWidget : public QWidget
{
    Q_OBJECT

public:
    explicit autoTypeWidget(QWidget *parent = nullptr);
    ~autoTypeWidget();

private:
    Ui::autoTypeWidget *ui;
    autoTypeWidgetPrivate* d;

};

#endif // AUTOTYPEWIDGET_H
