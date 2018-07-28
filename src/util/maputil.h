#ifndef MAPUTIL_H
#define MAPUTIL_H

#include <QPointF>
#include <QRectF>




namespace MapConst {
    const qreal MAX = 20037508.900383711000000;
    const qreal PI = 3.14159265358979323846;
    const qreal EARTH_R = 7371.393; // 6378.137
    const qreal _DegreeRadian = 3.14159265358979323846 / 180.0f;
    const qreal _RadianDegree = 1 / _DegreeRadian;
}

class MapUtil
{
public:

    /**
     * @brief MercatorToLgLa Mercator坐标转地理坐标
     * @param mercatorX 对应经度longitude
     * @param mercatorY 对应纬度latitude
     * @return
     */
    static QPointF getLgLaFromMercator(qreal mercatorX, qreal mercatorY);
    static QPointF getLgLaFromMercator(const QPointF& mercator);
    /**
     * @brief LgLaToMercator 地理坐标转Mercator坐标
     * @param longitude 经度，对应mercatorX
     * @param latitude 纬度，对应mercatorY
     * @return
     */
    static QPointF getMercatorFromLgLa(qreal longitude, qreal latitude);
    static QPointF getMercatorFromLgLa(const QPointF& lgla);

    /**
     * @brief getImageColLn 计算给定mercator、level下需要使用的瓦片的行列编号
     * @param mercator
     * @param level
     * @return 列(x)，行(y)，需要取整，小数部分为给定点在图片上的偏移量
     */
    static QPointF getImageColLn(const QPointF& mercator, int level);

    /**
     * @brief getImagePath 根据图片的行列、等级，计算出图片的路径
     * @param col
     * @param ln
     * @param level
     * @return
     */
    static QString getImagePath(int col, int row, int level);

    /**
     * @brief dist 两点之间的直线距离
     * @param p1
     * @param p2
     * @return
     */
    static qreal dist(const QPointF &p1, const QPointF &p2);

    /**
     * @brief getMercator 根据瓦片的行列、等级，计算瓦片top-left顶点mercator
     * @param col
     * @param ln
     * @param level
     * @param colOff
     * @param lnOff
     * @return
     */
    static QPointF getMercator(int col, int ln, int level);

    /**
     * @brief getDeltaMercatorOfOneColLn 计算当前Level下一个行列的差值对应的mercator坐标的差值
     * @param level
     * @return
     */
    static qreal getDeltaMercatorOfOneColLn(int level);

    static bool isRectOverlap(const QRectF& r1, const QRectF& r2);
    static bool isRectOverlap(qreal cx1, qreal cy1, qreal w1, qreal h1, qreal cx2, qreal cy2, qreal w2, qreal h2);

    static void getWebMercatorFromImageInfo(int level, int row, int col, QPointF *leftDwon, QPointF *rightTop);

    static bool isPointInRect(const QRectF& rect, const QPointF& point);

    /**
     * @brief ObjectLabel::crossPointOfRectAndLine 计算给定点到矩形中心连线与矩形边的交点
     * @param rectCentX 矩形中心x
     * @param rectCentY 矩形中心y
     * @param width     矩形宽度
     * @param height    矩形高度
     * @param x 给定点x
     * @param y 给定点y
     * @return 交点位置
     */
    static QPointF crossPointOfRectAndLine(float rectCentX, float rectCentY, float width, float height, float x, float y);

    /**
     * @brief getGeographyDist 计算给定经纬度间的距离
     * @return
     */
    static qreal getGeographyDist(const QPointF& lgla1, const QPointF& lgla2);
};

#endif // MATHUTIL_H
