#pragma once

#include <QMainWindow>
#include "QFileInfoList"
#include "../items/wpathitem.h"
#include "../items/wpolyitem.h"

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
    enum {CMBXSIZE = 20, ICONSIZE = 25, IMGSIZE = 128};
    explicit AiWidget(QWidget *parent = nullptr);
    ~AiWidget();

    /**
     * @brief 获取过滤后的image名称合集
     * @param 文件夹地址
     * @param 筛选的文件类型
     * @param 结果
     */
    int dirSetFilter(QString filepath, QString fileter, QFileInfoList &list);

    /**
     * @brief jsonRead:加载图形数据
     */
    void jsonRead();

    /**
     * @brief datatoHash:将当前item数据保存在hash中
     * @param index：要保存的序号
     * @param clean：是否清除scene上的item。
     */
    void datatoHash(const int index, const bool clean);

    /**
     * @brief 删除无用的item,并范围对应的zvalue数组
     */
    void clearEmptyItem(QList<int> &penZValue, QList<int> &polyZValue);

    /**
     * @brief 删除当前页的polyitem
     */
    void deletePolyItem(Graphics::WPolyItem *item);

    /**
     * @brief 删除当前页的pathitem
     */
    void deletePathItem(Graphics::WPathItem *item);

    /**
     * @brief saveOneBmp:保存单张图像
     * @param index：图像序号
     */
    int saveOneBmp(const int index);

    /**
     * @brief datapro获取所有item信息
     */
    void datatoDataPro();

private:
    /**
     * @brief 初始化图标
     */
    void iniIcon();

    /**
     * @brief 初始化布局
     */
    void iniLayout();

    /**
     * @brief 私有类数据和标注界面的初始化
     */
    void initialData();

private slots:
    /**
     * @brief 初始化进度栏对话框
     */
    void iniPd(const QString &text);

    /**
       @brief iniCombox:初始化颜色选择框
    */
    void iniCombox();

    /**
     * @brief changeImages: 切换图片显示序号
     */
    void changeImages(const int index);

    /**
     * @brief changeItems：更换相应页面的item
     * @param index :当前页面的序号
     */
    void changeItems(const int index);

    /**
     * @brief 更新item的zvalue
     */
    void updateZValue();

    /**
     * @brief showImage: 显示图片
     */
    void showImage(const int index);

    /**
     * @brief 清空scene上的item
     */
    void removeItem();

    /**
     * @brief getNullImage：获取全黑图，和原图的pixmap、
     */
    bool getNullImage(const int index, QPixmap &maskImage, QPixmap &image);

    /**
     * @brief isItemSelected: item是否被选中
     * @param penNum： pen被选中的序号
     * @param polyNum：poly被选中的序号
     * @return ：true 被选中， false 未被选择。
     */
    bool isItemSelected(int &penNum, int &polyNum);

    /**
     * @brief 修改所有pathItem的画笔宽度和线宽
     */
    void changeItemWidth(const int penWidth, const int eraserWidth);

private slots:
    void readImageTool();
    void saveOneTool(bool showDialog);
    void saveTool();
    void usePenTool();
    void useEraserTool();
    void usePolyTool();
    void deletePenTool();
    void useBiggerPenTool();
    void useSmallerPenTool();
    void itemBiggerTool();
    void itemSmallerTool();
    void allItemBiggerTool();
    void allItemSmallerTool();
    void changeAllColorTool();
    void iniChart();

private:
    Ui::AiWidget *ui;
    AiWidetPrivate *d;
};
