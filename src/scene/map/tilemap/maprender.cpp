#include "maprender.h"
#include <assert.h>
#include "allhead.h"

#define MAX_LEVEL 14
#define MIN_LEVEL 3
#define ADD_LEVEL_SCALE 1.8
#define DEL_LEVEL_SCALE 0.6
#define EQUAL_ZERO(x) !(x > 1e-10 || x < -1e-10)

SINGLETON_CPP(MapRender)

MapRender::~MapRender()
{

}

int MapRender::getLevel()
{
    return level;
}

QPointF MapRender::getCenterMercator()
{
    return centerMercator;
}

QPointF MapRender::getWindowPosFromMercator(QPointF mercator)
{
    return getWindowPosFromMercator(mercator.x(), mercator.y());
}
QPointF MapRender::getWindowPosFromMercator(qreal mercatorX, qreal mercatorY)
{
    qreal s = getPixelNumOfOneMercator();
    return QPointF(mercatorX - centerMercator.x(),
                   centerMercator.y() - mercatorY) * s + QPointF(w/2, h/2);
}

QPointF MapRender::getMercatorFromWindowPos(QPointF windowPos)
{
    qreal s = 1 / getPixelNumOfOneMercator();
    QPointF delta(windowPos.x() - w/2, -windowPos.y() + h/2);
    return centerMercator + delta * s;
}

const QMatrix &MapRender::getMercatorToPosMatrix()
{
    return mercatorToPosMatrix;
}

void MapRender::init()
{
    pressed = false;

    TileInfo tileInfo(0, 0, 1, QRectF(-MapConst::MAX, -MapConst::MAX, 2 * MapConst::MAX, 2 * MapConst::MAX));
    rootTile = QSharedPointer<MapTile>(new MapTile(NULL, tileInfo));

    qreal lg = Config::getValue("center/longitude", 117.563304).toDouble();
    qreal la = Config::getValue("center/latitude", 31.528802).toDouble();
    centerMercator = MapUtil::getMercatorFromLgLa(lg, la);
    level = Config::getValue("center/level", 8).toInt();
    scale = 1;
}

void MapRender::doOffset(QPointF offset)
{
    if(EQUAL_ZERO(offset.manhattanLength())){
        return;
    }
    qreal s = getPixelNumOfOneMercator();
    qreal x = offset.x() / s;
    qreal y = -offset.y() / s;

    centerMercator -= QPointF(x, y);
    adjustScaleAndOffset();
    updateWindowStatusParam();
}

void MapRender::doScale(const QPointF &pos, int direct)
{
    QPointF nowMercator = getMercatorFromWindowPos(pos);
    if(direct > 0){
        if(level != MAX_LEVEL){
            scale *= 1.1;
        }
    }else{
        if(level != MIN_LEVEL){
            scale *= 0.9;
        }
    }
    if(scale >= ADD_LEVEL_SCALE && level < MAX_LEVEL){
        level++;
        scale = 1;
    }
    if(scale <= DEL_LEVEL_SCALE && level > MIN_LEVEL){
        level--;
        scale = 1;
    }
    updateWindowStatusParam();
    adjustScaleAndOffset();
    moveMercatorToPos(nowMercator, pos);
}

/**
 * @brief MapRender::getPixelNumOfOneMercator 获取当前的缩放比例
 * @return 1 mercator 数值代表的像素个数
 */
qreal MapRender::getPixelNumOfOneMercator()
{
    return qMax(w, h) / (MapConst::MAX * 2) * scale * (1 << level) / (qMax(w, h) / 256 * 1.5);
}

void MapRender::updateWindowStatusParam()
{
    qreal s = getPixelNumOfOneMercator();
    // 一个像素代表的mercator
    qreal pix2Mer = 1 / s;

    // 稍微扩大视窗的范围，这样可以预加载不可见的地图
    qreal extendRatio = 1;

    windowMercatorRect = QRectF(- w / 2  * pix2Mer * extendRatio + centerMercator.x(),
                                - h / 2 * pix2Mer * extendRatio - centerMercator.y(),
                                w * pix2Mer * extendRatio,
                                h * pix2Mer * extendRatio);

    mercatorToPosMatrix.reset();
    mercatorToPosMatrix.translate(w/2 , h/2);
    mercatorToPosMatrix.scale(s, -s);
    mercatorToPosMatrix.translate(-centerMercator.x(), -centerMercator.y());
}

void MapRender::moveMercatorToPos(QPointF mercator, QPointF pos)
{
    QPointF currentPos = getWindowPosFromMercator(mercator);
    doOffset(pos - currentPos);
}

void MapRender::adjustScaleAndOffset()
{
    QPointF ld = getWindowPosFromMercator(QPointF(-MapConst::MAX, -MapConst::MAX));
    QPointF rt = getWindowPosFromMercator(QPointF(MapConst::MAX, MapConst::MAX));

    qreal s = getPixelNumOfOneMercator();
    if(s * 2 * MapConst::MAX < qMax(w, h)){
        scale = qMax(w, h) * scale / 2 / MapConst::MAX / s;
    }

    QPointF offset(0 , 0);

    if(rt.x() < w){
        offset.setX(w - rt.x());
    }
    if(rt.y() > 1){
        offset.setY(-rt.y());
    }
    if(ld.x() > 1){
        offset.setX(-ld.x());
    }
    if(ld.y() < h){
        offset.setY(h - ld.y());
    }

    doOffset(offset);
}

void MapRender::drawFrame(QPainter *painter)
{
    painter->save();
    {
        int needDrawNum = 0;
        rootTile->markNeedDraw(level, windowMercatorRect, &needDrawNum);

        painter->translate(w/2, h/2);
        qreal s = getPixelNumOfOneMercator();
        painter->scale(s, s);
        painter->translate(-centerMercator.x(), centerMercator.y());

        // 一帧绘制中，只可合成一张带矢量图的瓦片
        int drawVectorNum = 1;
        rootTile->drawFrame(painter, &drawVectorNum);

#ifdef DEBUG
        QPen p(QColor(Qt::green));
        p.setWidth(MapConst::MAX / 100 / ( 1 << (level-1)));
        painter->setPen(p);
        painter->drawRect(windowMercatorRect);
#endif
    }
    painter->restore();
}

bool MapRender::mousePressEvent(QMouseEvent *event, QPointF *mercator)
{
    pressed = true;
    pmouse = event->pos();

    qDebug()<<"mercator: "<<QString::number(mercator->x(),'f',5)<<","<<QString::number(mercator->y(),'f',5);

    return true;
}

bool MapRender::mouseReleaseEvent(QMouseEvent *event, QPointF *mercator)
{
    pressed = false;

    return false;
}

bool MapRender::mouseMoveEvent(QMouseEvent *event, QPointF *mercator)
{
    if(pressed){
        QPointF offset = event->pos() - pmouse;
        pmouse = event->pos();
        doOffset(offset);
    }
    return false;
}

void MapRender::windowSizeChange(int w, int h)
{
    this->w = w;
    this->h = h;
    updateWindowStatusParam();
}

void MapRender::wheelEvent(QWheelEvent *event)
{
    doScale(event->pos(), event->angleDelta().y());
}
