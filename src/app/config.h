#ifndef CONFIG_H
#define CONFIG_H

#include <QSettings>
#include <QVariant>
#include "cutil.h"

class Config
{
    SINGLETON_H(Config)
public:
    static void setValue(QString key, QVariant value);
    static QVariant getValue(QString key);
    static QVariant getValue(QString key, QVariant def);
    void init();
private:
    ~Config();
    QSettings *mSetting;
public:
    /***
     * 矢量地图文件信息
     **/
    struct Map{
        QString path;               //地图位置
        QString name;               //地图名称  没用，只是一个标识

        QString color;              //颜色 #ff0000格式，对应rgb为 255，0，0
        int maxLevel;               //需要显示的最大的LOD等级 只有在当前LOD等级处于最大与最小之间时才显示矢量地图
        int minLevel;               //需要显示的最小的LOD等级
        int pointSize;              //点大小
        int lineWidth;              //线宽度
        int fontSize;               //文字字体大小
        int priority;               //绘制优先级，优先级数值小先绘制，会被遮挡

        Map(QString name, QString path, QString color, int minLevel, int maxLevel, int pointSize, int lineWidth, int fontSize, int priority)
            :name(name), path(path), color(color), maxLevel(maxLevel), minLevel(minLevel), pointSize(pointSize), lineWidth(lineWidth), fontSize(fontSize), priority(priority)
        {}
        Map()
            :color("#ffffff"), maxLevel(1), minLevel(16), pointSize(2), lineWidth(2), fontSize(10), priority(0), name("")
        {}
    };
};

#endif // CONFIG_H
