#ifndef AIWIDGET_H
#define AIWIDGET_H

#include <QMainWindow>

namespace Ui {
class AiWidget;
}

/**
 * @brief The CSignalObj class: 信号类
 */
class CSignalObj : public QObject
{
    Q_OBJECT
public:
    CSignalObj() {}

signals:
    void sendErr(QString);
    void axisChange(int);
};

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
    void iniLayout();

private slots:
    void readImageTool();

private:
    Ui::AiWidget *ui;
    AiWidetPrivate *d;
};

#endif // AIWIDGET_H
