#ifndef TILEINFO_H
#define TILEINFO_H

#include <QRectF>
#include <QImage>
#include <QSharedPointer>
#include <QWeakPointer>


class TileInfo
{
public:
    QRectF rect;    // 绘制的矩形
    int row;        // 地图文件的行
    int col;        // 地图文件的列
    int level;      // 地图文件等级


    TileInfo(){}
    TileInfo(int row, int col, int level, QRectF rect);

    bool imageFileExist();
    QString imagePath();
    /**
     * @brief loadImage 尝试加载纹理
     * @return true:加载完成
     */
    QSharedPointer<QImage> loadImage(int* remainVectorMakeNum);
    bool operator ==(const TileInfo& t1) const;

    bool isVector;
    bool road;
    bool boundary;
    bool marksmp;
    bool markfull;
};

/**
 * hash值计算
 */
namespace std{
    template<>
    struct hash<TileInfo>{
        int operator()(const TileInfo& t) const{
            int result = t.level;
            result = 31 * result + t.row;
            result = 31 * result + t.col;
            if(t.isVector){
                result = 2 * result + t.road ? 1 : 0;
                result = 2 * result + t.boundary ? 1 : 0;
                result = 2 * result + t.marksmp ? 1 : 0;
                result = 2 * result + t.markfull ? 1 : 0;
            }
            return result;
        }
    };
}

#endif // TILEINFO_H
