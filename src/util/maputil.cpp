#include "maputil.h"
#include <QtMath>
#include <QPointF>
#include <QFile>
#include "config.h"
#include <QColor>

QPointF MapUtil::getLgLaFromMercator(qreal mercatorX, qreal mercatorY)
{
    qreal longitude = mercatorX / MapConst::MAX * 180;
    qreal latitude = mercatorY / MapConst::MAX * 180;
    latitude = 180 / MapConst::PI * (2 * qAtan(qExp(latitude * MapConst::PI / 180))- MapConst::PI / 2);
    return QPointF(longitude, latitude);
}

QPointF MapUtil::getLgLaFromMercator(const QPointF &mercator)
{
    return getLgLaFromMercator(mercator.x(), mercator.y());
}

QPointF MapUtil::getMercatorFromLgLa(qreal longitude, qreal latitude)
{
    qreal x = longitude * MapConst::MAX / 180;
    qreal y = qLn(qTan((90 + latitude) * MapConst::PI / 360)) / (MapConst::PI / 180);
    y = y * MapConst::MAX / 180;
    return QPointF(x, y);
}

QPointF MapUtil::getMercatorFromLgLa(const QPointF &lgla)
{
    return getMercatorFromLgLa(lgla.x(), lgla.y());
}

QPointF MapUtil::getImageColLn(const QPointF &mercator, int level)
{
    int rowNum = qPow(2, level - 1);
    qreal col = (mercator.x() + MapConst::MAX) / MapConst::MAX / 2 * rowNum;
    qreal ln = rowNum - (mercator.y() + MapConst::MAX ) / MapConst::MAX / 2 * rowNum;
    return QPointF(col, ln);
}

QString MapUtil::getImagePath(int col, int row, int level)
{
    QString rootpath = Config::getValue("tile/path", "G:/").toString();
    QString path = QString("%1LEVEL_%2/R%3/C%4.JPG")
            .arg(rootpath)
            .arg(level)
            .arg(row, 6, 10, QChar('0'))
            .arg(col, 6, 10, QChar('0'));
    if(QFile(path).exists()){
        return path;
    }

    QString rootpath1 = Config::getValue("tile/vicepath", "./Data/TileMap/").toString();
    QString path1 = QString("%1LEVEL_%2/R%3/C%4.JPG")
            .arg(rootpath1)
            .arg(level)
            .arg(row, 6, 10, QChar('0'))
            .arg(col, 6, 10, QChar('0'));
    return path1;
}

qreal MapUtil::dist(const QPointF& p1,const QPointF& p2)
{
    return qSqrt(qPow(p1.x() - p2.x(), 2) + qPow(p1.y() - p2.y(), 2));
}

QPointF MapUtil::getMercator(int col, int ln, int level)
{
    qreal rowNum = qPow(2, level - 1);
    qreal x = col * 2 * MapConst::MAX / rowNum - MapConst::MAX;
    qreal y = (rowNum - ln) / rowNum * 2 * MapConst::MAX - MapConst::MAX;
    return QPointF(x, y);
}

/**
 * @brief MapManager::GetWebMercatorOfTexture 根据纹理所在瓦片地图中的行列、LOD等级，计算对应Merctor坐标位置
 * @param level     LOD等级
 * @param row
 * @param col
 * @param leftDwon  左下角Mercator坐标
 * @param rightTop  右下角Mercator坐标
 */
void MapUtil::getWebMercatorFromImageInfo(int level, int row, int col, QPointF *leftDwon, QPointF *rightTop)
{
    int _maxRow = 1 << (level - 1);
    double _MAX = MapConst::MAX;
    double _dM = _MAX * 2 / _maxRow;
    *leftDwon = QPointF(-_MAX + col * _dM,         _MAX - _dM * (row +1));
    *rightTop = QPointF(-_MAX + (col+1) * _dM,     _MAX - _dM * row);
}

bool MapUtil::isPointInRect(const QRectF &rect, const QPointF &point)
{
    QPointF delta = rect.center() - point;
    return !(qAbs(delta.x() > rect.width() / 2) || qAbs(delta.y()) > rect.height() / 2);
}

qreal MapUtil::getGeographyDist(const QPointF &lgla1, const QPointF &lgla2)
{
    qreal radLat1 = qDegreesToRadians(lgla1.y());
    qreal radLat2 = qDegreesToRadians(lgla2.y());
    qreal a = radLat1 - radLat2;
    qreal b = qDegreesToRadians(lgla1.x()) - qDegreesToRadians(lgla2.x());
    qreal s = 2 * qAsin(qSqrt(qPow(qSin(a/2),2) +  qCos(radLat1)*qCos(radLat2)*qPow(qSin(b/2),2)));
    s = s * MapConst::EARTH_R * 1000;
    return s;
}

qreal MapUtil::getDeltaMercatorOfOneColLn(int level)
{
    int rowNum = qPow(2, level - 1);
    return MapConst::MAX * 2 / rowNum;
}

bool MapUtil::isRectOverlap(const QRectF &r1, const QRectF &r2)
{
    return qAbs(r1.center().x() - r2.center().x()) < (r1.width() + r2.width()) / 2
            && qAbs(r1.center().y() - r2.center().y()) < (r1.height() + r2.height()) / 2;
}

bool MapUtil::isRectOverlap(qreal cx1, qreal cy1, qreal w1, qreal h1, qreal cx2, qreal cy2, qreal w2, qreal h2){
    return qAbs(cx1 - cx2) < (w1 + w2) / 2
            && qAbs(cy1 - cy2) < (h1 + h2) / 2;
}


/**
 * @brief ObjectLabel::crossPointOfRectAndLine 计算给定点到矩形中心连线与矩形边的交点
 * @param rectCentX 矩形中心x
 * @param rectCentY 矩形中心y
 * @param width     矩形宽度
 * @param height    矩形高度
 * @param x 给定点x
 * @param y 给定点y
 * @param outX  交点x
 * @param outY  交点y
 */
QPointF MapUtil::crossPointOfRectAndLine(float rectCentX, float rectCentY, float width, float height, float x, float y)
{
    float dx = x - rectCentX;
    float dy = y - rectCentY;

    float outX, outY;

    //点在矩形内部
    if(std::abs(dx) < width/2 && std::abs(dy) < height/2)
    {
        return QPointF(x, y);
    }

    float tx, ty;

    if(dx == 0)
    {
        tx = 0;
        ty = height / 2 * (dy >= 0 ? 1 : -1);
    }
    else if(dy == 0)
    {
        ty = 0;
        tx = width / 2 * (dx >= 0 ? 1 : -1);
    }
    else if(std::abs(dx)/width >= std::abs(dy)/height)
    {
        tx = width / 2 * (dx >= 0 ? 1 : -1);
        ty = tx * dy / dx;
    }
    else
    {
        ty = height / 2 * (dy >= 0 ? 1 : -1);
        tx = ty * dx / dy;
    }
    outX = rectCentX + tx;
    outY = rectCentY + ty;

    return QPointF(outX, outY);
}
