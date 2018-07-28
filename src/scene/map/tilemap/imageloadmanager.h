#ifndef IMAGELOADMANAGER_H
#define IMAGELOADMANAGER_H

#include <QMap>
#include <QImage>
#include <QString>
#include <QRunnable>
#include <QList>
#include <QSharedPointer>
#include "cutil.h"
#include "tileinfo.h"
#include "imagelrucache.h"
#include <QMutex>
#include <QThreadPool>
#include <QAtomicInt>

// TODO O(1)的LruCache
struct CacheItem{
    TileInfo* tileInfo;
    QSharedPointer<QImage> cacheData;
    CacheItem(TileInfo* info, QSharedPointer<QImage> data){
        tileInfo = info;
        cacheData = data;
    }
};

/**
 * @brief The ImageLoadManager class 资源图片加载管理类
 */
class ImageLoadManager
{
    SINGLETON_H(ImageLoadManager)

public:
    void init();
    QSharedPointer<QImage> loadImage(const TileInfo& info);
    QSharedPointer<QImage> loadVector(const TileInfo& info);
    void cacheVectorMap(TileInfo info, QSharedPointer<QImage> vector);
private:
    ~ImageLoadManager();
    ImageLruCache mTileImageCache;
    ImageLruCache mVectorMapCache;

private:
    QThreadPool loadThreadPool;
    QAtomicInt currentTaskNum;

    class LoadImageTask : public QRunnable
    {
    public:
        LoadImageTask(TileInfo tileInfo, ImageLoadManager *manager);

        // QRunnable interface
    public:
        void run();

    private:
        ImageLoadManager* mManager;
        TileInfo mTileInfo;
    };

    friend class LoadImageTask;
};

#endif // IMAGELOADMANAGER_H
