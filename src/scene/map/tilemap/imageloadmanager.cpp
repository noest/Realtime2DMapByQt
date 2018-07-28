#include "imageloadmanager.h"
#include <QRunnable>
#include <QThreadPool>
#include <QImage>
#include <QFile>
#include <QMutex>
#include <QMutexLocker>
#include <QDebug>
#include "src/util/maputil.h"
#include "config.h"
#include "globalconst.h"
#include "vectorimagemaker.h"

SINGLETON_CPP(ImageLoadManager)

void ImageLoadManager::init()
{
    loadThreadPool.setMaxThreadCount(1);
    currentTaskNum = 0;
}

QSharedPointer<QImage> ImageLoadManager::loadImage(const TileInfo &info)
{
    QSharedPointer<QImage> img = mTileImageCache.get(info);
    if(!img.isNull()){
        return img;
    }

    currentTaskNum++;
    if(currentTaskNum > 5){
        loadThreadPool.clear();
        currentTaskNum = 0;
    }

    loadThreadPool.start(new LoadImageTask(info, this));
    return QSharedPointer<QImage>();
}

QSharedPointer<QImage> ImageLoadManager::loadVector(const TileInfo &info)
{
    if(loadImage(info).isNull()){
        return QSharedPointer<QImage>();
    }
    return mVectorMapCache.get(info);
}

void ImageLoadManager::cacheVectorMap(TileInfo info, QSharedPointer<QImage> vector)
{
    mVectorMapCache.set(info, vector);
}

ImageLoadManager::~ImageLoadManager()
{
}


ImageLoadManager::LoadImageTask::LoadImageTask(TileInfo tileInfo, ImageLoadManager* manager)
{
    mManager = manager;
    mTileInfo = tileInfo;
    setAutoDelete(true);
}

void ImageLoadManager::LoadImageTask::run()
{
    QString path = mTileInfo.imagePath();
    QFile file(path);
    QSharedPointer<QImage> image;
    if(file.exists()){
        image = QSharedPointer<QImage>(new QImage(
                                           GlobalConst::TILE_H,
                                           GlobalConst::TILE_W,
                                           QImage::Format_ARGB32));
        image->load(path);

    }
    mManager->mTileImageCache.set(mTileInfo, image);
    mManager->currentTaskNum--;
}
