/***
 * 该类用于读取和显示二维地图
 **/
#ifndef MAP2D_H
#define MAP2D_H

#include <map>
#include <vector>
#include <string>
#include <functional>
#include "mapshapes.h"
#include <QColor>
#include <QFont>
#include <QPointF>
#include <QPainter>
#include <QVector>
#include <QPen>
#include <QFont>
#include <QOpenGLBuffer>
#include <QOpenGLFunctions>

class OGRPoint;

class CMap2D
{
public:
    CMap2D(QOpenGLFunctions *func);
    virtual ~CMap2D();

public:
    //设置地图显示时的参数
    void setParam(QString name, QColor color, int minLevel = -1, int maxLevel = -1, float pointSize = -1, float lineWidth = -1, float fontSize = -1, int priority = 1);
    //加载给定目录下所有矢量图
    void loadAllMaps(QString const& floderName);

    void drawVectors(QPointF ld, QPointF rt, int level);
//    void drawVectorMarks(QPainter* painter);
    void drawVectorMarks(QPainter *painter, QPointF ld, QPointF rt, int level);
private:
    //加载矢量地图文件
    bool loadOGRData(QString filePath);

    //从地理坐标计算Mercator坐标
    MapPoint WebMercatorFromOGRPoint(OGRPoint const& point);

    //记录Mercator坐标内最大最小值
    void makeBound(const MapPoint& point);

    //初始化点、线VBO
    void createVertexBuffer();

    MapPoint minPoint,maxPoint; //记录矢量数据Merctor范围，超出范围的纹理不用执行绘制操作，节省时间

public:
    QColor color;               //使用的颜色
    QString name;
    int minLevel, maxLevel;     //显示地图的最大与最小LOD等级
    int pointSize;              //绘制时点的大小
    int lineWidth;              //绘制时线的宽度
    int fontSize;               //绘制时字体大小
    int priority;               //绘制优先级，数值小先绘制
    QPen mPen;
    QFont mFont;                //绘制时使用的字体

    GLuint VBO_Points;          //绘制点用的VBO
    GLuint VBO_Lines;           //绘制线使用的VBO

    std::vector<MapPoint> points;   //点
    std::vector<MapPoint> lines;    //线
    std::vector<MapMark> marks;     //地标
    int lineNum;
    int pointNum;

    QOpenGLFunctions* funcs;
};


#endif
