QT       += core gui
QT += charts
QT += concurrent

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11
qtHaveModule(printsupport): QT += printsupport
qtHaveModule(opengl): QT += opengl

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

include(aitool/aitool.pri)
include(camTool/camTool.pri)
include(items/items.pri)

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    wgraphicsscene.cpp \
    wgraphicsview.cpp \
    wgraphicswidget.cpp \
    cexecute.cpp

HEADERS += \
    mainwindow.h \
    wgraphicsscene.h \
    wgraphicsview.h \
    wgraphicswidget.h \
    cexecute.h\
    cfunction.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    icons.qrc

