#include "aidatapro.h"
#include "QSet"
#include "QPointF"
#include "QJsonArray"
#include "qjsondocument.h"
#include "qjsonobject.h"
#include "qjsonvalue.h"
#include "QDebug"
#include "QFileInfo"
#include "waiconstant.h"

class WDataProPrivate
{
public:
    WDataProPrivate() {}

    ~WDataProPrivate() {}

    QDir m_dir;
    QList<QString> m_filename;
    QList<QPixmap> m_images;
    QColor m_selectedColor;
    QList<QColor> m_preColor;
    QHash<int, ITEMQREAL> m_path;
    QHash<int, ITEMQREAL> m_poly;

};

WDataPro::WDataPro(): d(new WDataProPrivate())
{
    initialData();
}

void WDataPro::initialData()
{
    d->m_selectedColor = QColor(255, 0, 0);
    d->m_preColor.push_back(QColor(255, 0, 0));
    d->m_preColor.push_back(QColor(0, 255, 0));
    d->m_preColor.push_back(QColor(255, 255, 0));
    d->m_preColor.push_back(QColor(0, 0, 255));

    d->m_images.clear();
    d->m_filename.clear();
    initialItemData();
}

void WDataPro::initialItemData()
{
    d->m_poly.clear();
    d->m_path.clear();
}

void WDataPro::BubbleSort(QList<int> &in)
{
    if (in.isEmpty())
        return;
    int i, j, temp;
    for (j = 0; j < in.size() - 1; j++) {
        for (i = 0; i < in.size() - 1 - j; i++)
            if (in[i] > in[i + 1]) {
                temp = in[i];
                in[i] = in[i + 1];
                in[i + 1] = temp;
            }
    }
}

void WDataPro::BubbleSortChangeIndex(QList<int> &in)
{
    if (in.isEmpty())
        return;
    int i, j, temp, temp1;
    QList<int> logic;
    for (int index = 0 ; index < in.size(); index++)
        logic.push_back(index);

    for (j = 0; j < in.size() - 1; j++) {
        for (i = 0; i < in.size() - 1 - j; i++)
            if (in[i] < in[i + 1]) {
                temp = in[i];
                in[i] = in[i + 1];
                in[i + 1] = temp;

                temp1 = logic[i];
                logic[i] = logic[i + 1];
                logic[i + 1] = temp1;
            }
    }
    in.swap(logic);
}

void WDataPro::addFileName(const QString &fileName)
{
    d->m_filename.push_back(fileName);
}

void WDataPro::addImage(const QPixmap &img)
{
    d->m_images.push_back(img);
}

QList<QPixmap> WDataPro::getImages()
{
    return d->m_images;
}

void WDataPro::clearJsonFile()
{
    QString filename_poly, filename_path;
    QFileInfo fileinfo;
    for (int i = 0 ; i < d->m_filename.size() ; i++) {
        if (!d->m_path.contains(i)) {
            filename_path = d->m_dir.absolutePath() + "/path/" + d->m_filename[i] + ".json";
            fileinfo = QFileInfo(filename_path);
            if (fileinfo.isFile()) {
                QFile fileTemp(filename_path);
                fileTemp.remove();
            }
        }

        if (!d->m_poly.contains(i)) {
            filename_poly = d->m_dir.absolutePath() + "/poly/" + d->m_filename[i] + ".json";
            QFileInfo fileinfo = QFileInfo(filename_poly);
            if (fileinfo.isFile()) {
                QFile fileTemp(filename_poly);
                fileTemp.remove();
            }
        }
    }
}

void WDataPro::clearJsonFile(const int i)
{
    QString filename_poly, filename_path;
    QFileInfo fileinfo;

    if (!d->m_path.contains(i)) {
        filename_path = d->m_dir.absolutePath() + "/path/" + d->m_filename[i] + ".json";
        fileinfo = QFileInfo(filename_path);
        if (fileinfo.isFile()) {
            QFile fileTemp(filename_path);
            fileTemp.remove();
        }
    }

    if (!d->m_poly.contains(i)) {
        filename_poly = d->m_dir.absolutePath() + "/poly/" + d->m_filename[i] + ".json";
        QFileInfo fileinfo = QFileInfo(filename_poly);
        if (fileinfo.isFile()) {
            QFile fileTemp(filename_poly);
            fileTemp.remove();
        }
    }
}

QList<QString> WDataPro::getFileName() const
{
    return d->m_filename;
}

QDir WDataPro::getDir() const
{
    return d->m_dir;
}

void WDataPro::setDir(const QDir &dir)
{
    d->m_dir = dir;
}

void WDataPro::addItemInfo(const int type, const int index, const QVector<QVector<QVector<qreal> > > &info)
{
    if (!info.size()) return;

    if (ItemPath == type)
        d->m_path.insertMulti(index, info);

    if (ItemPoly == type)
        d->m_poly.insertMulti(index, info);
}

void WDataPro::getItemIndex(QList<int> &path, QList<int> &poly)
{
    path.clear();
    poly.clear();

    path = d->m_path.keys().toSet().toList();
    poly = d->m_poly.keys().toSet().toList();
}

QList<ITEMQREAL> WDataPro::getItemList(const int type, const int index)
{
    if (ItemPath == type) {
        return d->m_path.values(index);
    } else {
        return d->m_poly.values(index);
    }
}

QList<int> WDataPro::getDif(int totalNum, QList<int> saveSample)
{
    QList<int> total;
    QList<int> sub;
    for (int i = 0 ; i < totalNum ; i++)
        total.push_back(i);
    for (auto index : total) {
        if (!saveSample.contains(index))
            sub.push_back(index);
    }
    return sub;
}

int WDataPro::intSaveToJsonArray(ITEMINT item, QJsonArray &arr)
{
    QJsonArray jsonPointArray, jsonLineArray;
    if (item.size() == 0) return ErrorItem;
    while (!arr.isEmpty()) arr.removeFirst();
    for (auto itemhaha : item) {
        while (!jsonLineArray.isEmpty())
            jsonLineArray.removeFirst();
        for (auto linepoints : itemhaha) {
            while (!jsonPointArray.isEmpty())
                jsonPointArray.removeFirst();
            for (auto points : linepoints)
                jsonPointArray.append(points);
            jsonLineArray.append(jsonPointArray);
        }
        arr.append(jsonLineArray);
    }
    return OK;
}

int WDataPro::qrealSaveToJsonArray(ITEMQREAL item, QJsonArray &arr)
{
    QJsonArray jsonPointArray, jsonLineArray;
    if (item.size() == 0)
        return ErrorItem;
    while (!arr.isEmpty())
        arr.removeFirst();

    for (auto itemhaha : item) {
        while (!jsonLineArray.isEmpty())
            jsonLineArray.removeFirst();
        for (auto linepoints : itemhaha) {
            while (!jsonPointArray.isEmpty())
                jsonPointArray.removeFirst();
            for (auto points : linepoints)
                jsonPointArray.append(points);
            jsonLineArray.append(jsonPointArray);
        }
        arr.append(jsonLineArray);
    }
    return OK;
}


int WDataPro::pointsSaveToJsonArray(ITEMQPOINTF item, QJsonArray &arr)
{

    QJsonArray jsonPointArray, jsonLineArray;
    if (item.size() == 0)
        return ErrorItem;
    while (!arr.isEmpty())
        arr.removeLast();
    for (auto itemhaha : item) {
        while (!jsonLineArray.isEmpty())
            jsonLineArray.removeFirst();
        for (auto linepoints : itemhaha) {
            while (!jsonPointArray.isEmpty())
                jsonPointArray.removeFirst();
            for (auto points : linepoints) {
                jsonPointArray.append(points.x());
                jsonPointArray.append(points.y());
            }
            jsonLineArray.append(jsonPointArray);
        }
        arr.append(jsonLineArray);
    }
    return OK;
}

int WDataPro::dirSetFilter(QString filepath, QString fileter, QFileInfoList &list)
{
    QDir dir;
    list.clear();
    if (filepath.isEmpty()) return ErrorFilePath;
    dir = QDir(filepath);
    QStringList fileters;
    fileters = fileter.split(QRegExp("[ ]"));
    dir.setFilter(QDir::Files | QDir::NoSymLinks);
    dir.setSorting(QDir::Name);
    dir.setNameFilters(fileters);
    if (!dir.exists(filepath)) return ErrorFilePath;
    list = dir.entryInfoList();
    return OK;
}

int WDataPro::point2qreal(const QVector<QPointF> pts, QVector<qreal> &qPts)
{
    qPts.clear();

    for (int i = 0 ; i < pts.size() ; i++) {
        qPts.push_back(pts[i].x());
        qPts.push_back(pts[i].y());
    }
    return 0;
}

int WDataPro::qreal2Point(const QVector<qreal> qPts, QVector<QPointF> &pts)
{
    QPointF dian;
    pts.clear();

    if (qPts.size() % 2) return 1;

    for (int i = 0 ; i < qPts.size() ; i ++) {
        if (i % 2 == 0)
            dian.setX(qPts[i]);
        else {
            dian.setY(qPts[i]);
            pts.push_back(dian);
        }
    }
    return 0;
}

int WDataPro::color2qreal(const QColor color, QVector<qreal> &qlist)
{
    qlist.clear();
    qlist.push_back(color.redF());
    qlist.push_back(color.greenF());
    qlist.push_back(color.blueF());
    return 0;
}

int WDataPro::qreal2color(const QVector<qreal> &qlist, QColor color)
{
    if (qlist.size() != 3)
        return Error;
    color = QColor(qlist[0], qlist[1], qlist[2]);
    return 0;
}

int WDataPro::jsonArrayToPointArray(QJsonArray arrpoint, QVector<QVector<QVector<QPointF> > > &pointVector)
{
    QVector<QVector<QPointF>> twicepoints;
    QVector<QPointF> oncepoints;
    QPointF point;
    pointVector.clear();
    for (auto i : arrpoint) {
        QJsonArray arrItem = i.toArray();
        twicepoints.clear();
        for (auto j : arrItem) {
            QJsonArray arrLine = j.toArray();
            oncepoints.clear();
            for (int k = 0; k < arrLine.size(); k++) {
                qreal b = arrLine[k].toDouble();
                if (k % 2 == 0)
                    point.setX(b);
                else {
                    point.setY(b);
                    oncepoints.push_back(point);
                }
            }
            twicepoints.push_back(oncepoints);
        }
        pointVector.push_back(twicepoints);
    }
    return 0;
}

int WDataPro::jsonArrayToIntArray(QJsonArray arrpoint, ITEMINT &intVector)
{
    QVector<QVector<int>> twice;
    QVector<int> once;

    intVector.clear();
    for (auto i : arrpoint) {
        QJsonArray arrItem = i.toArray();
        twice.clear();
        for (auto j : arrItem) {
            once.clear();
            QJsonArray arrPoint = j.toArray();

            for (auto i : arrPoint)
                once.push_back(i.toInt());

            twice.push_back(once);
        }
        intVector.push_back(twice);
    }
    return 0;
}

int WDataPro::jsonArrayToQrealArray(QJsonArray arr, QVector<ITEMQREAL> &intVector)
{
    ITEMQREAL third;
    QVector<QVector<qreal>> twice;
    QVector<qreal> once;
    intVector.clear();

    for (auto j : arr) {
        QJsonArray arrItemvector = j.toArray();
        third.clear();
        for (auto i : arrItemvector) {
            QJsonArray arrItem = i.toArray();
            twice.clear();
            for (auto j : arrItem) {
                once.clear();
                QJsonArray arrPoint = j.toArray();
                for (auto i : arrPoint)
                    once.push_back(i.toDouble());
                twice.push_back(once);
            }
            third.push_back(twice);
        }
        intVector.push_back(third);
    }
    return 0;
}

int WDataPro::loadJsonFile(QFileInfoList list, int itemtype)
{
    QString strItem;

    if (list.size() < 0)
        return ErrorLoadJsonArray;

    if (itemtype == ItemPath) {
        strItem = "/path/";
        d->m_path.clear();
    } else if (itemtype == ItemPoly) {
        strItem = "/poly/";
        d->m_poly.clear();
    } else
        return ErrorLoadJsonArray;

    QJsonParseError json_error;
    for (auto i : list) {
        QString str = d->m_dir.absolutePath() + strItem + i.fileName();
        QFile loadFile(str);

        if (!loadFile.open(QIODevice::ReadOnly))
            return ErrorLoadJsonArray;

        QByteArray allData = loadFile.readAll();
        loadFile.close();
        QJsonDocument jsonDoc(QJsonDocument::fromJson(allData, &json_error));

        if (json_error.error != QJsonParseError::NoError)
            return ErrorLoadJsonArray;

        QJsonObject rootObj = jsonDoc.object();
        QVector<ITEMQREAL> itemInfo;

        for (int fileIndex = 0; fileIndex < d->m_filename.size(); fileIndex++) {
            if (rootObj.contains(d->m_filename[fileIndex])) {
                QJsonValue subObj = rootObj.value(d->m_filename[fileIndex]);
                QJsonArray arr = subObj.toArray();

                jsonArrayToQrealArray( arr, itemInfo);
                if (itemtype == ItemPath)
                    for (auto i : itemInfo)
                        d->m_path.insertMulti(fileIndex, i);
                else
                    for (auto i : itemInfo)
                        d->m_poly.insertMulti(fileIndex, i);
            }
        }
    }
    return OK;
}

int WDataPro::jsonWrite(const int index)
{
    if (d->m_dir.isEmpty()) return ErrorDir;

    QString filePath, filePoly;
    filePath = d->m_dir.absolutePath() + "/path";
    filePoly = d->m_dir.absolutePath() + "/poly";

    QJsonDocument jsonDocument;
    QJsonArray jsonTotalArray, jsonItemArray;
    QJsonObject jsonObj;
    QDir dir;

    //创建文件夹
    if (!dir.exists(filePath)) dir.mkpath(filePath);
    if (!dir.exists(filePoly)) dir.mkpath(filePoly);

    if (d->m_path.values(index).size()) {
        for (auto onesample : d->m_path.values(index)) {
            if (!qrealSaveToJsonArray(onesample, jsonItemArray))
                jsonTotalArray.append(jsonItemArray);
        }

        jsonObj.insert(d->m_filename[index], jsonTotalArray);
        jsonDocument.setObject(jsonObj);

        QFile savefile(filePath + "/" + d->m_filename[index] + ".json");
        if (!savefile.open(QIODevice::WriteOnly)) return ErrorItem;
        savefile.write(jsonDocument.toJson());
        savefile.close();
    }

    QJsonDocument jsonDocumentP;
    QJsonArray jsonTotalArrayP, jsonItemArrayP;
    QJsonObject jsonObjP;

    if (d->m_poly.values(index).size()) {
        for (auto onesample : d->m_poly.values(index)) {
            if (!qrealSaveToJsonArray(onesample, jsonItemArrayP))
                jsonTotalArrayP.append(jsonItemArrayP);
        }

        jsonObjP.insert(d->m_filename[index], jsonTotalArrayP);
        jsonDocumentP.setObject(jsonObjP);

        QFile savefile(filePoly + "/" + d->m_filename[index] + ".json");
        if (!savefile.open(QIODevice::WriteOnly)) return ErrorItem;
        savefile.write(jsonDocumentP.toJson());
        savefile.close();
    }
    return OK;
}
