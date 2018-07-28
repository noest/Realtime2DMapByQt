#include "tileinfo.h"
#include "maputil.h"
#include <QFile>
#include "imageloadmanager.h"
#include "vectorimagemaker.h"
#include "config.h"

TileInfo::TileInfo(int row, int col, int level, QRectF rect)
{
    this->row = row;
    this->col = col;
    this->level = level;
    this->rect = rect;
}

bool TileInfo::imageFileExist()
{
    return QFile(imagePath()).exists();
}

QString TileInfo::imagePath()
{
    return MapUtil::getImagePath(col, row, level);
}

// TODO 根据需要显示的资源配置调整
QSharedPointer<QImage> TileInfo::loadImage(int* remainVectorMakeNum)
{
    road = Config::getValue("vectormap/road", true).toBool();
    boundary = Config::getValue("vectormap/boundary", true).toBool();
    marksmp = Config::getValue("vectormap/marksmp", true).toBool();
    markfull = Config::getValue("vectormap/markfull", true).toBool();

    isVector = true;
    QSharedPointer<QImage> vec = ImageLoadManager::getInstance()->loadVector(*this);
    if(!vec.isNull()){
        return vec;
    }
    isVector = false;
    QSharedPointer<QImage> img = ImageLoadManager::getInstance()->loadImage(*this);
    if(!img.isNull()){
        if(*remainVectorMakeNum > 0){
            QImage* vec = VectorImageMaker::getInstance()->makeImage(img.data(), row, col, level);
            QSharedPointer<QImage> vecptr = QSharedPointer<QImage>(vec);
            isVector = true;
            ImageLoadManager::getInstance()->cacheVectorMap(*this, vecptr);
            (*remainVectorMakeNum)--;
            return vecptr;
        }
    }
    return img;
}

/**
 * 相等计算
 */
bool TileInfo::operator ==(const TileInfo &t1) const
{
    bool baseInfoEqual =  t1.col == col && t1.row == row && t1.level == level;
    if(!baseInfoEqual){
        return false;
    }
    if(!t1.isVector || !isVector){
        return true;
    }
    return t1.road == road && t1.boundary == boundary && t1.markfull == markfull && t1.marksmp == marksmp;
}
