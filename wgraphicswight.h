#ifndef WGRAPHICSWIGHT_H
#define WGRAPHICSWIGHT_H

#include <QFrame>
#include <QGraphicsView>
#include "wgraphicsview.h"

QT_BEGIN_NAMESPACE
class QLabel;
class QToolButton;
class QSlider;
QT_END_NAMESPACE

class WGraphicsWiget : public QFrame
{
    Q_OBJECT
public:
    explicit WGraphicsWiget(const QString &name, QWidget *parent = nullptr);
    QGraphicsView *view() const;

private slots:
    void resetView();
    void setResetButtonEnabled();
    void toggleOpenGL();
    void toggleAntialiasing();
    void print();

private:
    WGraphicsView *graphicsView;
    QLabel *label;
    QLabel *label2;
    QToolButton *selectModeButton;
    QToolButton *dragModeButton;
    QToolButton *openGlButton;
    QToolButton *antialiasButton;
    QToolButton *printButton;
    QToolButton *resetButton;
};

#endif // WGRAPHICSWIGHT_H
