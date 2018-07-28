/***
 * 矢量图元
 * */
#ifndef POLYGON_H
#define POLYGON_H

#include <vector>
#include <string>
#include <QPointF>
#include <QString>

//点
class MapPoint{
public:
    float x;
    float y;
    float z;
};
//地标，地名
class MapMark
{
public:
    std::string name;
    MapPoint position;
};
#endif // POLYGON_H
