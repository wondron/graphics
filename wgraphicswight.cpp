#include "wgraphicswight.h"

#if defined(QT_PRINTSUPPORT_LIB)
#include <QtPrintSupport/qtprintsupportglobal.h>
#if QT_CONFIG(printdialog)
#include <QPrinter>
#include <QPrintDialog>
#endif
#endif
#ifndef QT_NO_OPENGL
#include <QtOpenGL>
#else
#include <QtWidgets>
#endif
#include <QtMath>

WGraphicsWiget::WGraphicsWiget(const QString &name, QWidget *parent)
    : QFrame(parent)
{
    setFrameStyle(Sunken | StyledPanel);
    graphicsView = new WGraphicsView(this);
    graphicsView->setRenderHint(QPainter::Antialiasing, false);
    //graphicsView->setDragMode(QGraphicsView::RubberBandDrag);
    graphicsView->setOptimizationFlags(QGraphicsView::DontSavePainterState);
    graphicsView->setViewportUpdateMode(QGraphicsView::SmartViewportUpdate);
    graphicsView->setTransformationAnchor(QGraphicsView::AnchorUnderMouse);

    int size = style()->pixelMetric(QStyle::PM_ToolBarIconSize);
    QSize iconSize(size, size);

    resetButton = new QToolButton;
    QString str = u8"置零";
    resetButton->setText(str);
    resetButton->setEnabled(false);

    // Label layout
    QHBoxLayout *labelLayout = new QHBoxLayout;
    label = new QLabel(name);
    antialiasButton = new QToolButton;
    antialiasButton->setText(tr("Antialiasing"));
    antialiasButton->setCheckable(true);
    antialiasButton->setChecked(false);
    openGlButton = new QToolButton;
    openGlButton->setText(tr("OpenGL"));
    openGlButton->setCheckable(true);
#ifndef QT_NO_OPENGL
    openGlButton->setEnabled(QGLFormat::hasOpenGL());
#else
    openGlButton->setEnabled(false);
#endif
    printButton = new QToolButton;
    printButton->setIcon(QIcon(QPixmap("D:\\icon\\8010\\f1.ico")));

    labelLayout->addWidget(label);
    labelLayout->addStretch();
    labelLayout->addWidget(antialiasButton);
    labelLayout->addWidget(openGlButton);
    labelLayout->addWidget(printButton);
    labelLayout->addWidget(resetButton);

    QGridLayout *topLayout = new QGridLayout;
    topLayout->addLayout(labelLayout, 0, 0);
    topLayout->addWidget(graphicsView, 1, 0);
    setLayout(topLayout);

    connect(resetButton, &QAbstractButton::clicked, this, &WGraphicsWiget::resetView);
    connect(graphicsView->verticalScrollBar(), &QAbstractSlider::valueChanged,
            this, &WGraphicsWiget::setResetButtonEnabled);
    connect(graphicsView->horizontalScrollBar(), &QAbstractSlider::valueChanged,
            this, &WGraphicsWiget::setResetButtonEnabled);
    connect(antialiasButton, &QAbstractButton::toggled, this, &WGraphicsWiget::toggleAntialiasing);
    connect(openGlButton, &QAbstractButton::toggled, this, &WGraphicsWiget::toggleOpenGL);
    connect(printButton, &QAbstractButton::clicked, this, &WGraphicsWiget::print);
}

QGraphicsView *WGraphicsWiget::view() const
{
    return static_cast<QGraphicsView *>(graphicsView);
}

void WGraphicsWiget::resetView()
{
    graphicsView->ensureVisible(QRectF(0, 0, 0, 0));
    resetButton->setEnabled(false);
}

void WGraphicsWiget::setResetButtonEnabled()
{
    resetButton->setEnabled(true);
}

void WGraphicsWiget::toggleOpenGL()
{
#ifndef QT_NO_OPENGL
    graphicsView->setViewport(openGlButton->isChecked() ? new QGLWidget(QGLFormat(QGL::SampleBuffers)) : new QWidget);
#endif
}

void WGraphicsWiget::toggleAntialiasing()
{
    graphicsView->setRenderHint(QPainter::Antialiasing, antialiasButton->isChecked());
}

void WGraphicsWiget::print()
{
#if defined(QT_PRINTSUPPORT_LIB) && QT_CONFIG(printdialog)
    QPrinter printer;
    QPrintDialog dialog(&printer, this);
    if (dialog.exec() == QDialog::Accepted) {
        QPainter painter(&printer);
        graphicsView->render(&painter);
    }
#endif
}
