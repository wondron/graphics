#ifndef CITEMDATAPROC_H
#define CITEMDATAPROC_H
#include "qobject.h"
#include "QDir"
#include "QColor"
#include "QPixmap"

#define ITEMQREAL   QVector<QVector<QVector<qreal>>>
#define ITEMINT     QVector<QVector<QVector<int>>>
#define ITEMQPOINTF QVector<QVector<QVector<QPointF>>>

using namespace std;
class WDataProPrivate;
class WDataPro : public QObject
{

public:
    WDataPro();
    /**
     * @brief initialData: 数据初始化
     */
    void initialData();

    /**
     * @brief initialItemData: 清除item数据
     */
    void initialItemData();

    /**
     * @brief loadJsonFile:读取json文件信息
     * @param list：文件名
     * @param itemtype：item类型选择。
     */
    int loadJsonFile(QFileInfoList list, int itemtype);

    QList<int> getDif(int, QList<int>);

    /**
     * @brief jsonWrite: 保存json文件
     * @param index：图像序号
     */
    int jsonWrite(const int index);

    /**
     * @brief BubbleSort : 冒泡排序
     */
    void BubbleSort(QList<int> &in);

    /**
     * @brief BubbleSort : 冒泡排序
     */
    void BubbleSortChangeIndex(QList<int> &in);

    /**
     * @brief 添加file名
     */
    void addFileName(const QString &fileName);

    /**
     * @brief addImage: 添加图像
     */
    void addImage(const QPixmap &img);

    /**
     * @brief getImages
     * @return
     */
    QList<QPixmap> getImages();

    /**
     * @brief 删除data保存的json文件
     */
    void clearJsonFile();

    /**
     * @brief 删除data保存的json文件
     */
    void clearJsonFile(const int i);

    QList<QString> getFileName() const;

    QDir getDir() const;
    void setDir(const QDir &dir);

    void addItemInfo(const int type, const int index, const ITEMQREAL &info);
    /**
     * @brief getItemList: 获取特定序号的itemList
     * @param type：类型：ItemPath 和 ItemPoly
     * @param index： 序号
     */
    QList<ITEMQREAL> getItemList(const int type, const int index);
    /**
     * @brief getItemIndex：获取item在那些序号上
     */
    void getItemIndex(QList<int> &path, QList<int> &poly);

private:

    int qrealSaveToJsonArray(ITEMQREAL item, QJsonArray &arr);

    int intSaveToJsonArray(ITEMINT item, QJsonArray &arr);

    int pointsSaveToJsonArray(ITEMQPOINTF item, QJsonArray &arr);

    int jsonArrayToPointArray(QJsonArray arr, ITEMQPOINTF &pointVector);

    int jsonArrayToIntArray(QJsonArray arr, ITEMINT &intVector);

    int jsonArrayToQrealArray(QJsonArray arr, QVector<ITEMQREAL> &intVector);

public:
    /**
     * @brief dirSetFilter:文件目录过滤器
     * @param filepath：文件夹路径
     * @param fileter：筛选条件
     * @param list：各文件路径
     * @return ok or ng
     */
    static int dirSetFilter(QString filepath, QString fileter, QFileInfoList &list);

    /**
     * @brief point2qreal: 点坐标转换为实数数组
     * @param pts： 点数组
     * @param qPts： 实数数组
     */
    static int point2qreal(const QVector<QPointF> pts, QVector<qreal> &qPts);

    /**
     * @brief qreal2Point: 实数数组转换为点坐标
     * @param qPts： 实数数组
     * @param pts：  点集
     */
    static int qreal2Point(const QVector<qreal> qPts, QVector<QPointF> &pts);

    /**
     * @brief color2qreal: 颜色转换成实数数组
     */
    static int color2qreal(const QColor color, QVector<qreal> &qlist);

    /**
     * @brief color2qreal: 实数数组转换成颜色
     */
    static int qreal2color(const QVector<qreal> &qlist, QColor color);

private:
    WDataProPrivate *d;
};

#endif // CITEMDATAPROC_H
