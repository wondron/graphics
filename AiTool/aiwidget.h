#ifndef AIWIDGET_H
#define AIWIDGET_H

#include <QMainWindow>

namespace Ui {
class AiWidget;
}

class AiWidetPrivate;
class AiWidget : public QMainWindow
{
    Q_OBJECT

public:
    enum {ICONSIZE = 25};
    explicit AiWidget(QWidget *parent = nullptr);
    ~AiWidget();

private:
    void iniIcon();

private:
    Ui::AiWidget *ui;
    AiWidetPrivate *d;
};

#endif // AIWIDGET_H
