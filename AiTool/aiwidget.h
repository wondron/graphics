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
    explicit AiWidget(QWidget *parent = nullptr);
    ~AiWidget();

private:
    Ui::AiWidget *ui;
    AiWidetPrivate *d;
};

#endif // AIWIDGET_H
