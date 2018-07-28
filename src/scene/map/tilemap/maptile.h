#ifndef MAPTILE_H
#define MAPTILE_H

#include "tileinfo.h"

/**
 * @brief The MapTile class 地图上的一个瓦片
 */
class MapTile
{
public:
    MapTile(MapTile* parent, const TileInfo &info);

    // IRender interface
public:
    bool markNeedDraw(int level, const QRectF& windowMercatorRect, int *drawTileCount);
    void drawFrame(QPainter *painter, int *onceDrawRemainCount);
private:
    void makeChild();
    QSharedPointer<MapTile> m_child[4];
    TileInfo m_info;
    MapTile* m_parent;
    bool m_needDraw;
    bool m_childNeedDraw;
    bool m_childinited;
};

#endif // MAPTILE_H
