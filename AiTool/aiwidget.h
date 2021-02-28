#ifndef AIWIDGET_H
#define AIWIDGET_H

#include <QMainWindow>

namespace Ui {
class AiWidget;
}

class AiWidget : public QMainWindow
{
    Q_OBJECT

public:
    explicit AiWidget(QWidget *parent = nullptr);
    ~AiWidget();

private:
    Ui::AiWidget *ui;
};

#endif // AIWIDGET_H
