#include "vectorimagemaker.h"
#include <QPainter>
#include "map2d.h"
#include <QMutexLocker>
#include <tr1/functional>
#include "src/util/maputil.h"
#include <QOpenGLFunctions>
#include "config.h"
#include "maputil.h"

SINGLETON_CPP(VectorImageMaker)

bool VectorImageMaker::init(int width, int height)
{
    m_device = new QOpenGLPaintDevice(width, height);
    m_context = m_device->context();
    m_function = m_context->functions();
    m_function->initializeOpenGLFunctions();

    windowWidth = width;
    windowHeight = height;

    QOpenGLFramebufferObjectFormat format;
    format.setSamples(4);
    format.setAttachment(QOpenGLFramebufferObject::Depth);

    m_fbo = new QOpenGLFramebufferObject(width, height, format);

    initVectorMap();
    return m_fbo->isValid();
}

QImage *VectorImageMaker::makeImage(QImage *srcImg, int row, int col, int level)
{
    m_fbo->bind();
    QPainter painter(m_device);
    painter.setPen(Qt::white);

    // 绘制原图片
    painter.drawImage(QRect(0, 0, windowWidth, windowHeight), *srcImg);

    // 绘制地标名称
    QPointF ld, rt;
    MapUtil::getWebMercatorFromImageInfo(level, row, col, &ld, &rt);
    drawVectorMarks(&painter, ld, rt, level);

    //    // 绘制图片信息
    //    painter.drawText(QRect(0, 0, w, h), QString("rt: %1 - %2")
    //                     .arg(QString::number(rt.x(),'f',6))
    //                     .arg(QString::number(rt.y(),'f',6)));

    //    painter.drawText(QRect(0, 10, w, h), QString("ld: %1 - %2")
    //                     .arg(QString::number(ld.x(),'f',6))
    //                     .arg(QString::number(ld.y(),'f',6)));


    // 使用vbo绘制线
    painter.beginNativePainting();
    {
        glViewport(0, 0, windowWidth, windowHeight);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(0, windowWidth, 0, windowHeight, -1, 1);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        drawVectorMap(ld, rt, level);
    }
    painter.endNativePainting();

    QImage* outImg = new QImage(m_fbo->toImage());

    m_fbo->release();
    return outImg;
}

QList<CMap2D *> VectorImageMaker::getMaps()
{
    return maps;
}

VectorImageMaker::~VectorImageMaker()
{
    m_fbo->release();
    delete m_fbo;
    delete m_device;
    foreach(CMap2D* map, maps){
        delete map;
    }
}

void VectorImageMaker::drawVectorMap(QPointF ld, QPointF rt, int level)
{
    foreach(CMap2D* map, maps){
        map->drawVectors(ld, rt, level);
    }
}

void VectorImageMaker::drawVectorMarks(QPainter *painter, QPointF ld, QPointF rt, int level)
{
    foreach(CMap2D* map, maps){
        map->drawVectorMarks(painter, ld, rt, level);
    }
}

void VectorImageMaker::initVectorMap()
{
    QList<Config::Map> mapsInfo;
    mapsInfo.append(Config::Map("marksmp","./Data/VectorMap/mark_simple",Config::getValue("color/provinceName","#aaffff88").toString(), 6, 16, 2, 2, 16, 2));
    mapsInfo.append(Config::Map("markfull","./Data/VectorMap/mark_full",Config::getValue("color/palceName","#aac3c369").toString(), 7, 16, 2, 2, 12, 2));
    mapsInfo.append(Config::Map("boundary","./Data/VectorMap/boundary",Config::getValue("color/boundary","#aabfbfaf").toString(), 1, 16, 1, 2, 14, 1));
    mapsInfo.append(Config::Map("road","./Data/VectorMap/road",Config::getValue("color/road","#aa9F9F8F").toString(),8,16,1,2,12,1));


    for(int i = 0 ; i < mapsInfo.size() ; i++)
    {
        CMap2D *map = new CMap2D(m_function);
        Config::Map mapinfo = mapsInfo.at(i);
        map->setParam(mapinfo.name, QColor(mapinfo.color), mapinfo.minLevel, mapinfo.maxLevel, mapinfo.pointSize, mapinfo.lineWidth, mapinfo.fontSize, mapinfo.priority);
        map->loadAllMaps(mapinfo.path);
        maps.append(map);
    }

    qSort(maps.begin(), maps.end(), [](CMap2D* a, CMap2D* b)->bool{
        return a->priority < b->priority;
    });
}


