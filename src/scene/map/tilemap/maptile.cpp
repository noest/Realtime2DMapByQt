#include "maptile.h"
#include <QPainter>
#include <QPaintEvent>
#include <random>
#include "src/util/maputil.h"
#include <QFile>
#include <QThreadPool>
#include <QDebug>
#include <QOpenGLBuffer>
#include "vectorimagemaker.h"
#include "tileinfo.h"
#include "maprender.h"

MapTile::MapTile(MapTile *parent, const TileInfo& info)
{
    m_parent = parent;
    m_info = info;
    m_childinited = false;
}

/**
 * @brief MapTile::makeChild
 * c0       c1
 * c2       c3
 */
void MapTile::makeChild()
{
    TileInfo c[] = {
        // c0
        TileInfo(m_info.row * 2, m_info.col * 2, m_info.level + 1,
        QRectF(m_info.rect.left(), m_info.rect.top(), m_info.rect.width() / 2, m_info.rect.height() / 2)),
        // c1
        TileInfo(m_info.row * 2, m_info.col * 2 + 1, m_info.level + 1,
        QRectF(m_info.rect.center().x(), m_info.rect.top(), m_info.rect.width() / 2, m_info.rect.height() / 2)),
        // c2
        TileInfo(m_info.row * 2 + 1, m_info.col * 2, m_info.level + 1,
        QRectF(m_info.rect.left(), m_info.rect.center().y(), m_info.rect.width() / 2, m_info.rect.height() / 2)),
        // c3
        TileInfo(m_info.row * 2 + 1, m_info.col * 2 + 1, m_info.level + 1,
        QRectF(m_info.rect.center().x(), m_info.rect.center().y(), m_info.rect.width() / 2, m_info.rect.height() / 2)),
    };
    for(int i = 0 ; i < 4; i++){
        if(c[i].imageFileExist()){
            m_child[i] = QSharedPointer<MapTile>(new MapTile(this, c[i]));
        }
    }
}

/**
 * @brief MapTile::markNeedDraw 标记是否可绘制，是否有子节点需要绘制
 */
bool MapTile::markNeedDraw(int level, const QRectF& windowMercatorRect, int *drawTileCount)
{
    m_needDraw = true;
    m_childNeedDraw = false;

    if(level < m_info.level){
        m_needDraw = false;
        return false;
    }

    if(!MapUtil::isRectOverlap(m_info.rect, windowMercatorRect)){
        m_needDraw = false;
        return false;
    }

    if(!m_childinited){
        makeChild();
        m_childinited = true;
        return false;
    }

    int drawChildNum = 0;
    for(int i = 0; i < 4 ;i++){
        QSharedPointer<MapTile> child = m_child[i];
        if(!child.isNull()){
            if(child->markNeedDraw(level, windowMercatorRect, drawTileCount)){
                drawChildNum++;
            }
        }
    }

    if(drawChildNum > 0){
        m_childNeedDraw = true;
    }

    // 4个子节点都可以绘制时，自身不绘制
    if(drawChildNum == 4){
        m_needDraw = false;
    }
    if(m_needDraw){
        (*drawTileCount)++;
    }
    return m_needDraw || m_childNeedDraw;
}

/**
 * @brief MapTile::drawFrame
 * @param painter
 * @param onceDrawRemainCount 本次绘制中可合成的vectorMap剩余数量
 */
void MapTile::drawFrame(QPainter *painter, int* onceDrawRemainCount)
{
    if(m_needDraw){
        QSharedPointer<QImage> img = m_info.loadImage(onceDrawRemainCount);
        if(!img.isNull()){
            painter->drawImage(m_info.rect, *img);
        }
#ifdef DEBUG
        QPen p(QColor(Qt::red));
        p.setWidth(MapConst::MAX / 100 / (1 << m_info.level));
        painter->setPen(p);
        painter->drawRect(m_info.rect);
//        qDebug()<<m_info.imagePath();
#endif
    }
    if(m_childNeedDraw){
        for(int i = 0; i < 4 ;i++){
            QSharedPointer<MapTile> child = m_child[i];
            if(!child.isNull()){
                child->drawFrame(painter, onceDrawRemainCount);
            }
        }
    }
}
