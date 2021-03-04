#include "aiwidget.h"
#include "ui_aiwidget.h"
#include "wgraphicswidget.h"
#include "QDebug"
#include "qfiledialog.h"
#include "QMessageBox"
#include "aidatapro.h"
#include "cexecute.h"
#include "QProgressDialog"
#include "QColorDialog"

#include "WAiConstant.h"

#include <QtCharts/QChartView>
#include <QtCharts/QSplineSeries>
#include <QValueAxis>
#include <QtConcurrent/QtConcurrent>
#include <QChart>

QT_CHARTS_USE_NAMESPACE

#define CHECKERR(ifstr, errStr)\
if (ifstr){\
emit d->signalObj->sendErr(errStr);\
return;\
}

#define SETICONBTN(btnName, tip, funcName, shortcut) \
connect(btnName, &QPushButton::clicked, this, &AiWidget::funcName);\
btnName->setShortcut(QString(shortcut));\
btnName->setToolTip(tip);

using namespace Graphics;

class AiWidetPrivate
{
public:
    AiWidetPrivate(AiWidget *pp): p(pp)
    {
        fileLabel = new QLabel(u8"当前文件夹：");
        srcWidget = new WGraphicsWidget();
        labelWidget = new WGraphicsWidget();
        m_dataPro = new WDataPro();
        pd = new QProgressDialog("", u8"取消", 0, 100);
        pd->setValue(100);
    }
    ~AiWidetPrivate()
    {
        delete srcWidget;
        delete labelWidget;
        delete signalObj;
        delete fileLabel;
        delete m_dataPro;
        delete pd;
    }

    AiWidget *p;

    //界面类成员
    QLabel *fileLabel;
    WGraphicsWidget *srcWidget;
    WGraphicsWidget *labelWidget;
    WDataPro *m_dataPro;

    //item类数据成员
    QList<QListWidgetItem *> m_item;
    QList<WPolyItem *> m_poly;  //当前页的item对象
    QList<WPathItem *> m_path;
    QHash<int, WPolyItem *> m_widgetPoly;  //所有的item对象
    QHash<int, WPathItem *> m_widgetPath;

    //zValue操作
    int zValue = 850;
    QList<int> polyzValue;  //当前页的zValue
    QList<int> pathzValue;

    //处理序号
    int m_lastIndex = 0;
    int m_index = 0;
    bool firstClickSwitch = true;

    //其他
    QColor m_Color;
    QList<QColor> preColor;
    QProgressDialog *pd;
    CSignalObj *signalObj = new CSignalObj();

    //深度学习方面对象
    QChartView *chartView;
    QSplineSeries *series;
    QValueAxis *m_xAxis;
    QValueAxis *m_yAxis;
    int m_trainIndex;
    QChart *chart;
    QTimer *timer;
};


AiWidget::AiWidget(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::AiWidget), d(new AiWidetPrivate(this))
{
    ui->setupUi(this);
    iniLayout();
    iniIcon();
    //iniCombox();

    //信号对象的连接
    connect(d->signalObj, &CSignalObj::sendErr, [this](QString a) {
        QMessageBox::question(this, u8"提示", a, QMessageBox::Ok);
    });

    connect(d->srcWidget->view(), &Graphics::WGraphicsView::dragChanged, [ = ]() {
        QPointF center = d->srcWidget->view()->mapToScene(d->srcWidget->view()->rect().center().x(), d->srcWidget->view()->rect().center().y());
        d->labelWidget->view()->centerOn(center.x(), center.y());
    });

    connect(d->labelWidget->view(), &Graphics::WGraphicsView::dragChanged, [ = ]() {
        QPointF center = d->labelWidget->view()->mapToScene(d->labelWidget->view()->rect().center().x(), d->labelWidget->view()->rect().center().y());
        d->srcWidget->view()->centerOn(center.x(), center.y());
    });

    connect(d->labelWidget->view(), &Graphics::WGraphicsView::scaleChanged, d->srcWidget->view(), &Graphics::WGraphicsView::zoom);
    connect(d->srcWidget->view(), &Graphics::WGraphicsView::scaleChanged, d->labelWidget->view(), &Graphics::WGraphicsView::zoom);

    connect(ui->listWidget, &QListWidget::clicked, [this](const QModelIndex & index) {
        d->m_lastIndex = d->m_index;
        d->m_index = index.row();
        if (index.row() > d->m_dataPro->getImages().size()) return;
        if (!d->m_dataPro->getImages().size()) return;
        changeImages(d->m_index);
    });
}

AiWidget::~AiWidget()
{
    delete ui;
}

int AiWidget::dirSetFilter(QString filepath, QString fileter, QFileInfoList &list)
{
    QDir dir;
    list.clear();
    if (filepath.isEmpty()) return 1;
    dir = QDir(filepath);
    QStringList fileters;
    fileters = fileter.split(QRegExp("[ ]"));
    dir.setFilter(QDir::Files | QDir::NoSymLinks);
    dir.setSorting(QDir::Name);
    dir.setNameFilters(fileters);
    if (!dir.exists(filepath)) return 1;
    list = dir.entryInfoList();
    return 0;
}

void AiWidget::jsonRead()
{
    QFileInfoList list;
    QString strpoly = d->m_dataPro->getDir().absolutePath() + "/poly";
    QString strpath = d->m_dataPro->getDir().absolutePath() + "/path";

    if (!WDataPro::dirSetFilter(strpoly, "*.json", list)) {
        if (d->m_dataPro->loadJsonFile(list, itemType::ItemPoly))
            emit d->signalObj->sendErr(u8"画笔数据读取错误！");
    }

    if (!WDataPro::dirSetFilter(strpath, "*.json", list)) {
        if (d->m_dataPro->loadJsonFile(list, itemType::ItemPath))
            emit d->signalObj->sendErr(u8"多边形数据读取错误！");
    }

    QList<int> mpathIndex, mpolyIndex;
    d->m_dataPro->getItemIndex(mpathIndex, mpolyIndex);

    for (auto i : mpathIndex) {
        for (auto j : d->m_dataPro->getItemList(ItemPath, i)) {
            auto item = new WPathItem();
            item->setItemInfo(j);
            d->m_widgetPath.insertMulti(i, item);
        }
    }

    for (auto i : mpolyIndex) {
        for (auto j : d->m_dataPro->getItemList(ItemPoly, i)) {
            auto itempoly = new WPolyItem();
            itempoly->setItemInfo(j);
            d->m_widgetPoly.insertMulti(i, itempoly);
        }
    }
}

void AiWidget::datatoHash(const int index, const bool clean)
{
    //首次点击返回
    if (d->firstClickSwitch || index < 0) {
        d->firstClickSwitch = false;
        return;
    }
    d->labelWidget->currentScene()->clearSelection();
    clearEmptyItem(d->pathzValue, d->polyzValue);

    //对清理过的item进行重新编号。
    int zvalue = 850;
    QList<int> totalzValue;
    totalzValue = d->polyzValue + d->pathzValue;
    d->m_dataPro->BubbleSort(totalzValue);
    totalzValue = totalzValue.toSet().toList();
    d->m_dataPro->BubbleSort(totalzValue);
    for (auto i : totalzValue) {
        for (int index = 0; index < d->polyzValue.size() ; index ++) {
            if (d->m_poly[index]->zValue() == i) {
                d->m_poly[index]->setZValue(zvalue++);
            }
        }
        for (int index = 0; index < d->pathzValue.size() ; index ++) {
            if (d->m_path[index]->zValue() == i) {
                d->m_path[index]->setZValue(zvalue++);
            }
        }
    }

    //结果保存到QHash中，并根據clear判断是否移除界面上的item。
    d->m_widgetPath.remove(index);
    for (auto item : d->m_path)
        d->m_widgetPath.insertMulti(index, item);

    d->m_widgetPoly.remove(index);
    for (auto item : d->m_poly)
        d->m_widgetPoly.insertMulti(index, item);

    if (clean) removeItem();
}

void AiWidget::clearEmptyItem(QList<int> &penZValue, QList<int> &polyZValue)
{
    penZValue.clear();
    polyZValue.clear();

    //删除没有数据的item
    for (int i = d->m_path.size() - 1; i >= 0 ; i--) {
        if (!d->m_path[i]->isEmpty()) {
            deletePathItem(d->m_path[i]);
            d->m_path.removeAt(i);
        } else {
            penZValue.push_back(d->m_path[i]->zValue());
        }
    }
    for (int i = d->m_poly.size() - 1; i >= 0 ; i--) {
        if (!(d->m_poly[i]->isEmpty())) {
            deletePolyItem(d->m_poly[i]);
            d->m_poly.removeAt(i);
        } else {
            polyZValue.push_back(d->m_poly[i]->zValue());
        }
    }
}

void AiWidget::deletePolyItem(WPolyItem *item)
{
    item->setBoundingRect(9000, 9000);
    d->labelWidget->currentScene()->removeItem(item);
    delete item;
    d->labelWidget->currentScene()->update();
}

void AiWidget::deletePathItem(WPathItem *item)
{
    item->setBoundingRect(9000, 9000);
    d->labelWidget->currentScene()->removeItem(item);
    delete item;
    d->labelWidget->currentScene()->update();
}

int AiWidget::saveOneBmp(const int index)
{
    //item提取
    QList<WPolyItem *> poly = d->m_widgetPoly.values(index);
    QList<WPathItem *> path = d->m_widgetPath.values(index);
    int itemNum = path.size() + poly.size();

    //文件夹检测。
    QDir dir;
    QString fileName = d->m_dataPro->getDir().absolutePath();
    QString maskPath = fileName + "/mask";
    QString labelPath = fileName + "/label";
    if (!dir.exists(maskPath)) dir.mkpath(maskPath);
    if (!dir.exists(labelPath)) dir.mkpath(labelPath);

    //获取空图像和原图
    QPixmap mask, image;
    if (!getNullImage(index, mask, image))
        return Error;
    if (itemNum) {
        QPainter painterItem, painterAll;
        QList<int> polyZValue, pathZValue, zValue;

        painterItem.begin(&mask);
        painterAll.begin(&image);

        //获取zValue数组；
        for (auto i : path) pathZValue.push_back(i->zValue());
        for (auto i : poly) polyZValue.push_back(i->zValue());
        zValue = polyZValue + pathZValue;
        d->m_dataPro->BubbleSort(zValue);

        //根据zValue顺序画图形
        for (auto i : zValue) {
            //画poly
            if (polyZValue.contains(i)) {
                int polyIndex = polyZValue.indexOf(i);
                QColor co = poly[polyIndex]->color();
                QPainterPath pathItem;
                QPolygonF polyItem = QPolygonF(poly[polyIndex]->qvectorPoints());
                painterItem.setPen(QPen(co, 1));
                pathItem.addPolygon(polyItem);
                painterItem.fillPath(pathItem, co);
                painterAll.fillPath(pathItem, co);
            }
            //画path
            if (pathZValue.contains(i)) {
                int pathIndex = pathZValue.indexOf(i);
                painterItem.setPen(QPen(path[pathIndex]->color(), 1));
                path[pathIndex]->paint(&painterItem, nullptr, nullptr);
                path[pathIndex]->paint(&painterAll, nullptr, nullptr);
            }
        }
        painterItem.end();
        painterAll.end();
    }
    QString file = "/" + d->m_dataPro->getFileName()[index];
    mask.save(maskPath + file);
    image.save(labelPath + file);
    return OK;
}

void AiWidget::datatoDataPro()
{
    d->m_dataPro->initialItemData();

    QList <int> pathIndex = d->m_widgetPath.keys().toSet().toList();
    QList<int> polyIndex = d->m_widgetPoly.keys().toSet().toList();

    for (auto index : pathIndex) {
        for ( auto item : d->m_widgetPath.values(index))
            d->m_dataPro->addItemInfo(ItemPath, index, item->itemInfo());
    }
    for (auto index : polyIndex) {
        for (auto item : d->m_widgetPoly.values(index))
            d->m_dataPro->addItemInfo(ItemPoly, index, item->itemInfo());
    }
}

void AiWidget::iniIcon()
{
    QString str = ":/aiImageIcon/aiImgIcon/back.png";
    ui->btn_back->setIconSize(QSize(ICONSIZE * 2, ICONSIZE / 2));
    ui->btn_back->setIcon(QIcon(QPixmap(str)));

    str = ":/aiImageIcon/aiImgIcon/next.png";
    ui->btn_next->setIconSize(QSize(ICONSIZE * 2, ICONSIZE / 2));
    ui->btn_next->setIcon(QIcon(QPixmap(str)));

    QStringList list;
    list << "open" << "save" << "saveOne"
         << "pen" << "polygon" << "delete" << "eraser";

    QList<QPushButton *> btnList;
    btnList << ui->btn_open << ui->btn_save << ui->btn_saveOne
            << ui->btn_pen << ui->btn_polygon << ui->btn_delete << ui->btn_eraser;

    for (int i = 0 ; i < list.size() ; i ++) {
        str = QString(":/aiImageIcon/aiImgIcon/%1.png").arg(list[i]);
        btnList[i]->setIcon(QIcon(QPixmap(str)));
        btnList[i]->setIconSize(QSize(ICONSIZE, ICONSIZE));
        btnList[i]->setFixedSize(QSize(ICONSIZE + 4, ICONSIZE + 4));
    }

    SETICONBTN(ui->btn_open, u8"打开文件夹(f)", readImageTool, "f");
    SETICONBTN(ui->btn_save, u8"保存当前图片(3)", saveOneTool, "3");
    SETICONBTN(ui->btn_saveOne,  u8"全部保存(ctrl+shift+s)", saveTool, "ctrl+shift+s");
    SETICONBTN(ui->btn_pen,  u8"画笔工具(b)", usePenTool, "b");
    SETICONBTN(ui->btn_polygon,  u8"多边形工具(n)", usePolyTool, "n");
    SETICONBTN(ui->btn_eraser,  u8"橡皮擦(e)", useEraserTool, "e");
    //SETICONBTN(biggerPen,  u8"增加画笔宽度(a)", useBiggerPenTool, "a");
    //SETICONBTN(smallerPen,  u8"减小画笔宽度(s)", useSmallerPenTool, "s");
    SETICONBTN(ui->btn_delete, u8"删除(d)", deletePenTool, "d");
    //SETICONBTN(itemBigger, u8"变宽当前画笔线条(9)", itemBiggerTool, "9");
    //SETICONBTN(itemSmaller, u8"变窄当前画笔线条(8)", itemSmallerTool, "8");
    //SETICONBTN(allBigger, u8"变宽所有画笔线条(2)", allItemBiggerTool, "2");
    //SETICONBTN(allSmall, u8"变窄所有画笔线条(5)", allItemSmallerTool, "5");
    //SETICONBTN(allChangecolor, u8"改变所有颜色(c)", changeAllColorTool, "c");
}

void AiWidget::iniLayout()
{
    ui->hlay_src->addWidget(d->srcWidget);
    ui->hlay_label->addWidget(d->labelWidget);
    ui->statusbar->addWidget(d->fileLabel);

    ui->listWidget->setIconSize(QSize(IMGSIZE, IMGSIZE));
    ui->listWidget->setResizeMode(QListView::Adjust);
    ui->listWidget->setViewMode(QListView::IconMode);
    ui->listWidget->setMovement(QListView::Static);
    ui->listWidget->setSpacing(10);

    d->labelWidget->setLabelShow(false);
    d->srcWidget->setLabelShow(false);

    d->labelWidget->setAimShow(false);
    d->srcWidget->setAimShow(false);

    ui->grpBx_model->setVisible(false);
}

void AiWidget::initialData()
{
    removeItem();
    d->m_dataPro->initialData();

    d->m_item.clear();
    d->m_poly.clear();
    d->m_path.clear();
    d->m_widgetPoly.clear();
    d->m_widgetPath.clear();
    d->firstClickSwitch = true;
    ui->listWidget->clear();
    d->m_lastIndex = 0;
    d->m_index = 0;
    ui->m_hSlidereraser->setEnabled(false);
    ui->m_hSlidereraser->setValue(5);
    ui->m_hSliderpen->setEnabled(false);
    ui->m_hSliderpen->setValue(1);
    ui->label_pen->setText(QString::asprintf(u8"画笔大小: %d", ui->m_hSliderpen->value()));
}

void AiWidget::iniPd(const QString &text)
{
    d->pd->reset();
    d->pd->setLabelText(text);
    d->pd->setWindowTitle(u8"请稍等");
    d->pd->setWindowModality(Qt::WindowModal);
    d->pd->setFont(QFont("微软雅黑"));
    d->pd->show();
}

void AiWidget::iniCombox()
{
    ui->cmbx_colorSlct->setIconSize(QSize(CMBXSIZE, CMBXSIZE));
    QPixmap pixmap(20, 20);
    pixmap.fill(d->preColor[0]);
    ui->cmbx_colorSlct->insertItem(0, QIcon(pixmap), "");
    pixmap.fill(d->preColor[1]);
    ui->cmbx_colorSlct->insertItem(1, QIcon(pixmap), "");
    pixmap.fill(d->preColor[2]);
    ui->cmbx_colorSlct->insertItem(2, QIcon(pixmap), "");
    pixmap.fill(d->preColor[3]);
    ui->cmbx_colorSlct->insertItem(3, QIcon(pixmap), "");
    ui->cmbx_colorSlct->setCurrentIndex(0);
}

void AiWidget::changeImages(const int index)
{
    datatoHash(d->m_lastIndex, true);
    showImage(index);
    changeItems(index);
    updateZValue();

    if (ui->chkbx_autoSave->isChecked()) {
        QtConcurrent::run([ = ]() {
            d->m_dataPro->clearJsonFile(d->m_lastIndex);
            int a = 0;
            a += saveOneBmp(d->m_lastIndex);
            a += d->m_dataPro->jsonWrite(d->m_lastIndex);
            QString msg = a ? u8"单张图像保存失败" : u8"单张图像保存成功";
            qDebug() << msg;
        });
    }
}

void AiWidget::changeItems(const int index)
{
    if (d->m_widgetPoly.contains(index)) {
        for (auto item : d->m_widgetPoly.values(index)) {
            d->labelWidget->currentScene()->addItem(item);
            item->turnoffSwitch();
            item->changeBoundingRect();
            d->m_poly.push_back(item);
            d->polyzValue.push_back(item->zValue());
        }
    }
    if (d->m_widgetPath.contains(index)) {
        for (auto item : d->m_widgetPath.values(index)) {
            //ui->m_hSlidereraser->isEnabled() ? item->setEraserMode() : item->setDrawMode();
            //item->setPenWidth(ui->m_hSliderpen->value());
            //item->setEraserWidth(ui->m_hSlidereraser->value());
            d->labelWidget->currentScene()->addItem(item);
            item->changeBoundingRect();
            d->m_path.push_back(item);
            d->pathzValue.push_back(item->zValue());
        }
    }
}

void AiWidget::updateZValue()
{
    QList<int> total = d->pathzValue + d->polyzValue;
    d->m_dataPro->BubbleSort(total);
    if (total.isEmpty())
        d->zValue = 850;
    else
        d->zValue = total.at(total.size() - 1) + 1;
}

void AiWidget::showImage(const int index)
{
    d->pathzValue.clear();
    d->polyzValue.clear();

    ui->listWidget->setCurrentRow(index);
    d->srcWidget->setImage(d->m_dataPro->getImages()[index].toImage());
    d->labelWidget->setImage(d->m_dataPro->getImages()[index].toImage());
}

void AiWidget::removeItem()
{
    for (auto i : d->m_poly) {
        i->setBoundingRect(9000, 9000);
        d->labelWidget->currentScene()->removeItem(i);
    }

    for (auto i : d->m_path) {
        i->setBoundingRect(9000, 9000);
        d->labelWidget->currentScene()->removeItem(i);
    }

    d->m_path.clear();
    d->m_poly.clear();
}

bool AiWidget::getNullImage(const int index, QPixmap &maskImage, QPixmap &image)
{
    if (index >= d->m_dataPro->getFileName().size())
        return false;

    image =  d->m_dataPro->getImages()[index].copy();

    int width, height;
    width = image.width();
    height = image.height();

    QPixmap pixNull(width, height);
    pixNull.fill(Qt::black);
    maskImage = pixNull.copy();
    return true;
}

bool AiWidget::isItemSelected(int &penNum, int &polyNum)
{
    bool selected = false;
    penNum = -1;
    polyNum = -1;
    for (int i = 0 ; i < d->m_path.size() ; i++) {
        if (d->m_path[i]->isSelected()) {
            selected = true;
            penNum = i;
        }
    }
    for (int i = 0 ; i < d->m_poly.size() ; i++) {
        if (d->m_poly[i]->isSelected()) {
            selected = true;
            polyNum = i;
        }
    }
    return selected;
}

void AiWidget::changeItemWidth(const int penWidth, const int eraserWidth)
{
    if (!d->m_path.size()) return;

    if (penWidth > 99 || eraserWidth > 99) return;

    if (penWidth > 0) {
        for ( auto i : d->m_path)
            i->setPenWidth(penWidth);
    }

    if (eraserWidth > 0) {
        for (auto i : d->m_path)
            i->setEraserWidth(eraserWidth);
    }
}

void AiWidget::readImageTool()
{
    QString directory = QFileDialog::getExistingDirectory(this, u8"选择图片文件夹");
    CHECKERR(directory == "", u8"所选文件夹为空");

    QFileInfoList list;
    d->fileLabel->setText(u8"当前操作文件夹：" + directory);
    d->m_dataPro->setDir(QDir(directory));
    int err = WDataPro::dirSetFilter(directory, "*.bmp *.jpg *.png", list);
    CHECKERR(err, u8"读取图像失败");
    CHECKERR(list.size() <= 0, u8"图像数量为零！");

    int listsize = ui->listWidget->size().width() - 40;
    initialData();

    QtConcurrent::run([ = ]() {
        for (int i = 0; i < list.size(); i++) {
            d->m_dataPro->addFileName(list[i].fileName());
            QPixmap objPixmap(list[i].absoluteFilePath());
            qDebug() << list[i];
            qreal pixra = (((qreal)objPixmap.height()) / ((qreal)objPixmap.width()));
            d->m_dataPro->addImage(objPixmap);
            Async::CExecute(true) <= [ = ]() {
                QListWidgetItem *item = new QListWidgetItem(QIcon(objPixmap.scaled(QSize(listsize, (int)listsize * pixra))), list[i].fileName());
                item->setToolTip(list[i].fileName());
                ui->listWidget->addItem(item);
                d->m_item.push_back(item);
                d->pd->setValue(100 * i / list.size());
            };
            QThread::msleep(10);
        }
        Async::CExecute(true) <= [ = ]() {
            jsonRead();
            d->pd->setValue(100);
        };
    });
}

void AiWidget::saveOneTool(bool showDialog)
{
    CHECKERR(!d->m_dataPro->getFileName().size(), u8"请先打开图片！");
    QtConcurrent::run([ = ]() {
        datatoHash(d->m_index, false);
        datatoDataPro();
        d->m_dataPro->clearJsonFile(d->m_index);
        int a = 0;
        qDebug() << "start save one bmp, index:" << d->m_index;
        a += saveOneBmp(d->m_index);
        a += d->m_dataPro->jsonWrite(d->m_index);
        QString msg = a ? u8"单张图像保存失败" : u8"单张图像保存成功";
        if (!showDialog) {
            qDebug() << msg;
        } else {
            Async::CExecute(true) <= [ = ]() {
                emit d->signalObj->sendErr(msg);
            };
        }
    });
}

void AiWidget::saveTool()
{
    CHECKERR(!d->m_dataPro->getFileName().size(), u8"请先打开图片！");
    iniPd(u8"正在保存数据");
    QtConcurrent::run([ = ]() {
        datatoHash(d->m_index, false);
        datatoDataPro();
        d->m_dataPro->clearJsonFile();
        int a = 0;
        int size = d->m_dataPro->getFileName().size();
        for (int i = 0; i < size; i++) {
            a += saveOneBmp(i);
            a += d->m_dataPro->jsonWrite(i);
            Async::CExecute(true) <= [ = ]() {
                d->pd->setValue(100 * i / size);
            };
        }
        QString msg = a ? u8"图像保存失败" : u8"图像保存成功";
        Async::CExecute(true) <= [ = ]() {
            d->pd->setValue(100);
            emit d->signalObj->sendErr(msg);
        };
    });
}

void AiWidget::usePenTool()
{
    CHECKERR(!d->m_dataPro->getFileName().size(), u8"请先打开图片！");

    bool itemSame = false;
    int penIndex, polyIndex;
    ui->m_hSliderpen->setEnabled(true);
    ui->m_hSlidereraser->setEnabled(false);

    itemSame = isItemSelected(penIndex, polyIndex);
    if (itemSame && (penIndex >= 0)) {
        for (auto i : d->m_path) i->setDrawMode();
        d->m_path[penIndex]->setPenWidth(ui->m_hSliderpen->value());
    } else {
        auto itemPen = new WPathItem();
        itemPen->setDrawMode();
        itemPen->setPenWidth(ui->m_hSliderpen->value());
        itemPen->setColor(d->m_Color);
        d->pathzValue.push_back(d->zValue);
        itemPen->setZValue(d->zValue++);
        d->labelWidget->currentScene()->addItem(itemPen);
        d->m_path.push_back(itemPen);
    }
}

void AiWidget::useEraserTool()
{
    CHECKERR(!d->m_dataPro->getFileName().size(), u8"请先打开图片！");

    ui->m_hSliderpen->setEnabled(false);
    ui->m_hSlidereraser->setEnabled(true);
    for (auto i : d->m_path) {
        i->setEraserMode();
        i->setEraserWidth(ui->m_hSlidereraser->value());
    }
}

void AiWidget::usePolyTool()
{
    CHECKERR(!d->m_dataPro->getFileName().size(), u8"请先打开图片！");

    auto item = new WPolyItem();
    item->setColor(d->m_Color);
    d->polyzValue.push_back(d->zValue);
    item->setZValue(d->zValue++);
    d->labelWidget->currentScene()->addItem(item);
    d->m_poly.push_back(item);
}

void AiWidget::deletePenTool()
{
    QMessageBox::StandardButton btnResult;
    CHECKERR(!(d->m_path.size() + d->m_poly.size()), u8"请先创建画笔工具！");
    int pathselectedIndex, polyselectedIndex;
    bool isSelected = isItemSelected(pathselectedIndex, polyselectedIndex);

    if (!isSelected) {
        btnResult = QMessageBox::question(this, "Tip", u8"请确定删除全部?", QMessageBox::Ok | QMessageBox::Cancel);
        if (btnResult == QMessageBox::Cancel) return;
        for (auto i : d->m_path) deletePathItem(i);
        for (auto i : d->m_poly) deletePolyItem(i);
        d->m_path.clear();
        d->m_poly.clear();
    } else {
        if (pathselectedIndex >= 0) {
            deletePathItem(d->m_path[pathselectedIndex]);
            d->m_path.removeAt(pathselectedIndex);
        } else {
            deletePolyItem(d->m_poly[polyselectedIndex]);
            d->m_poly.removeAt(polyselectedIndex);
        }
    }
}

void AiWidget::useBiggerPenTool()
{
    CHECKERR(!d->m_dataPro->getFileName().size(), u8"请先打开图片！");
    int penSlct, polySlct, width;
    isItemSelected(penSlct, polySlct);
    if (penSlct < 0) return;

    WPathItem *item = d->m_path[penSlct];

    if (item->isDrawMode()) {
        width = item->penWidth() + 3;
        ui->m_hSliderpen->setValue(width);
        changeItemWidth(width, -1);
    } else {
        width = item->eraserWidth() + 3;
        ui->m_hSlidereraser->setValue(width);
        changeItemWidth(-1, width);
    }
}

void AiWidget::useSmallerPenTool()
{
    CHECKERR(!d->m_dataPro->getFileName().size(), u8"请先打开图片！");
    int penSlct, polySlct, width;
    isItemSelected(penSlct, polySlct);
    if (penSlct < 0) return;

    WPathItem *item = d->m_path[penSlct];

    if (item->isDrawMode()) {
        width = item->penWidth() - 3;
        ui->m_hSliderpen->setValue(width);
        changeItemWidth(width, -1);
    } else {
        width = item->eraserWidth() - 3;
        ui->m_hSlidereraser->setValue(width);
        changeItemWidth(-1, width);
    }
}

void AiWidget::itemBiggerTool()
{
    CHECKERR(!d->m_path.size(), u8"请先添加画笔工具");

    int penslct, a;
    isItemSelected(penslct, a);

    if (penslct < 0) return;
    d->m_path[penslct]->allBigger();
}

void AiWidget::itemSmallerTool()
{
    CHECKERR(!d->m_path.size(), u8"请先添加画笔工具");

    int penslct, a;
    isItemSelected(penslct, a);

    if (penslct < 0) return;
    d->m_path[penslct]->allSmaller();
}

void AiWidget::allItemBiggerTool()
{
    CHECKERR(!d->m_path.size(), u8"请先添加画笔工具");

    for (auto i : d->m_path) {
        i->allBigger();
    }
}

void AiWidget::allItemSmallerTool()
{
    CHECKERR(!d->m_path.size(), u8"请先添加画笔工具");

    for (auto i : d->m_path)
        i->allSmaller();
}

void AiWidget::changeAllColorTool()
{
    CHECKERR(!(d->m_path.size() + d->m_poly.size()), u8"请先添加画笔工具");

    QMessageBox::StandardButton btnResult = QMessageBox::question(this, "Tips",
                                            u8"确定改变所有颜色？",
                                            QMessageBox::Ok | QMessageBox::Cancel);
    if (btnResult == QMessageBox::Cancel) return;

    d->m_Color = QColorDialog::getColor(d->m_Color);
    for (auto i : d->m_path) {
        i->setColor(d->m_Color);
    }
    for (auto i : d->m_poly)
        i->setColor(d->m_Color);

    d->labelWidget->currentScene()->clearSelection();
    d->labelWidget->currentScene()->update();
}

void AiWidget::iniChart()
{

}
