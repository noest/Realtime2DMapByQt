#ifndef IMAGELRUCACHE_H
#define IMAGELRUCACHE_H

#include "lrucache.h"
#include "tileinfo.h"
#include <QImage>
#include <QSharedPointer>

class ImageLruCache
        : public LruCache<TileInfo,QSharedPointer<QImage> >
{
public:
    ImageLruCache();
};

#endif // IMAGELRUCACHE_H
