#ifndef MAPRENDER_H
#define MAPRENDER_H

#include "src/scene/irender.h"
#include <QVector>
#include "src/scene/map/tilemap/maptile.h"
#include <QPointF>
#include <QBrush>
#include "maptile.h"
#include "cutil.h"

/**
 * @brief The MapRender class 整个地图绘制类
 */
class MapRender: public IRender
{
    Q_OBJECT
    SINGLETON_H(MapRender)
public:
    ~MapRender();
    int getLevel();
    QPointF getCenterMercator();

    /**
     * @brief getWindowPosOfMercator 获取给定mercator在屏幕上的坐标
     */
    QPointF getWindowPosFromMercator(QPointF mercator);
    QPointF getWindowPosFromMercator(qreal mercatorX, qreal mercatorY);
    /**
     * @brief getMercatorOfWindowPos 获取指定屏幕坐标的mercator
     */
    QPointF getMercatorFromWindowPos(QPointF windowPos);
    /**
     * @brief getMercatorToPosMatrix 获取可将mercator坐标转换为屏幕坐标的矩阵
     */
    const QMatrix &getMercatorToPosMatrix();
    void init();
    qreal getPixelNumOfOneMercator();
private:
    // 地图参数
    QPointF centerMercator;
    QRectF windowMercatorRect;
    QMatrix mercatorToPosMatrix;
    int level;

    QSharedPointer<MapTile> rootTile;

    // 交互函数
    void doOffset(QPointF offset);
    void doScale(const QPointF& pos, int direct);
    void updateWindowStatusParam();
    void moveMercatorToPos(QPointF mercator, QPointF pos);
    void adjustScaleAndOffset();


    // IRender interface
public:
    void drawFrame(QPainter *painter);
    bool mousePressEvent(QMouseEvent *event, QPointF *mercator);
    bool mouseReleaseEvent(QMouseEvent *event, QPointF *mercator);
    bool mouseMoveEvent(QMouseEvent *event, QPointF *mercator);
    void windowSizeChange(int w, int h);
    void wheelEvent(QWheelEvent *event);

private:
    // 交互参数
    int w, h;
    QPointF pmouse;
    bool pressed;
    qreal scale;
};

#endif // MAPRENDER_H
