#include "map2d.h"
#include <QDir>
#include <QDebug>
#include "src/util/maputil.h"
#include <QFontDatabase>
#include "Dependencies/gdal/include/OGR/ogrsf_frmts.h"
#include <QTextCodec>
#include <QPainter>
#include "config.h"
#include "globalconst.h"
#include <QOpenGLFunctions>
#include "maprender.h"

CMap2D::CMap2D(QOpenGLFunctions* func)
{
    color = QColor(1,1,0);
    minLevel = 1;
    maxLevel = 16;
    pointSize = 3;
    lineWidth = 2;
    fontSize = 12;
    mFont = QFont(Config::getValue("vectormap/font","楷体").toString());

    minPoint.x = MapConst::MAX;
    minPoint.y = MapConst::MAX;
    maxPoint.x = -MapConst::MAX;
    maxPoint.y = - MapConst::MAX;

    points.clear();
    lines.clear();
    marks.clear();

    funcs = func;
}

CMap2D::~CMap2D()
{
    if(funcs != NULL){
        funcs->glDeleteBuffers(1,&VBO_Lines);
        funcs->glDeleteBuffers(1,&VBO_Points);
    }
    marks.clear();
}

/**
 * @brief CMap2D::setParam 设置绘制时的各种参数
 * @param color
 * @param minLevel
 * @param maxLevel
 * @param pointSize
 * @param lineWidth
 * @param fontSize
 * @param priority
 */
void CMap2D::setParam(QString name, QColor color, int minLevel, int maxLevel, float pointSize, float lineWidth, float fontSize, int priority)
{
    this->name = name;
    this->color = color;
    this->minLevel = minLevel > 0 ? minLevel : this->minLevel;
    this->maxLevel = maxLevel > 0 ? maxLevel : this->maxLevel;
    this->pointSize = pointSize > 0 ? pointSize : this->pointSize;
    this->lineWidth = lineWidth > 0 ? lineWidth : this->lineWidth;
    this->fontSize = fontSize > 0 ? fontSize : this->fontSize;
    this->priority = priority > 0 ? priority : this->priority;

    mPen = QPen(color);
    mPen.setWidth(lineWidth);
    mFont.setPointSize(fontSize);
}

/**
 * @brief CMap2D::loadAllMaps 根据所给的路径，遍历目录内所有tab文件，并加载每个文件
 * @param floderName 目录路径
 */
void CMap2D::loadAllMaps(const QString &floderName)
{
    QDir dir(floderName);
    if (!dir.exists())
    {
        return;
    }

    QStringList filters;
    filters<<QString("*.tab")<<QString("*.TAB");

    dir.setFilter(QDir::Files | QDir::NoSymLinks);
    dir.setNameFilters(filters);

    QFileInfoList list = dir.entryInfoList();
    for(int i = 0 ; i < list.size() ; i++)
    {
        loadOGRData(list.at(i).absoluteFilePath());
    }

    createVertexBuffer();
}

/**
 * @brief CMap2D::drawVectors 执行矢量图的绘制操作，绘制到当前FBO中
 * @param ld    图片左下角mercator
 * @param rt    图片右上角mercator
 * @param level
 */
void CMap2D::drawVectors(QPointF ld, QPointF rt, int level)
{
    //绘制范围控制，不在范围内不绘制
    if(level > maxLevel || level < minLevel) return;
    if(maxPoint.x < ld.x() || maxPoint.y < ld.y()
            || minPoint.x > rt.x() || minPoint.y > rt.y()) return;

    QString setname = QString("vectormap/%1").arg(name);
    if(!Config::getValue(setname, true).toBool()){
        return;
    }


    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    double deltaX = rt.x() - ld.x();
    double deltaY = rt.y() - ld.y();

    glScalef(256 / deltaX, 256 / deltaY, 1);            //将绘制范围压缩大256x256范围内
    glTranslatef((float)-ld.x(), (float)-ld.y(), 0);    //使得0，0位置对应纹理左下角

    glColor3f(color.redF(), color.greenF(), color.blueF());
    funcs->glEnableVertexAttribArray(0);
    {
        //点绘制
        funcs->glBindBuffer(GL_ARRAY_BUFFER, VBO_Points);
        funcs->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
        glDrawArrays(GL_POINTS, 0, pointNum);
        funcs->glBindBuffer(GL_ARRAY_BUFFER, 0);

        //线绘制
        funcs->glBindBuffer(GL_ARRAY_BUFFER, VBO_Lines);
        funcs->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
        glDrawArrays(GL_LINES, 0, lineNum);
        funcs->glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
    funcs->glDisableVertexAttribArray(0);
}

//void CMap2D::drawVectorMarks(QPainter *painter)
//{
//    int level = MapRender::getInstance()->getLevel();
//    if(level > maxLevel || level < minLevel) return;

//    painter->setFont(mFont);
//    int w = Config::getValue("window/width").toInt();
//    int h = Config::getValue("window/height").toInt();
//    foreach(MapMark mark, marks){
//        QPointF pos = MapRender::getInstance()->getWindowPosFromMercator(mark.position.x, mark.position.y);
//        if(qAbs(pos.x() - w / 2) > w * 1.1
//                || qAbs(pos.y() - h / 2) > h * 1.1){
//            continue;
//        }
//        painter->setFont(mFont);
//        painter->setPen(mPen);
//        painter->drawText(QRect(pos.x(), pos.y(), 256, 256),
//                          QString::fromStdString(mark.name));
//    }
//}

void CMap2D::drawVectorMarks(QPainter *painter, QPointF ld, QPointF rt, int level)
{
    if(level > maxLevel || level < minLevel) return;

    QString setname = QString("vectormap/%1").arg(name);
    if(!Config::getValue(setname, true).toBool()){
        return;
    }

    QPointF delta = rt - ld;
    QPointF center = (rt + ld) / 2;
    foreach(MapMark mark, marks){
        qreal x = mark.position.x;
        qreal y = mark.position.y;
        // 放宽范围控制，使得跨了图片的地名能完全绘制
        if(qAbs(x - center.x()) > delta.x() || qAbs(y - center.y()) > delta.y()){
            continue;
        }
        painter->setFont(mFont);
        painter->setPen(mPen);
        painter->drawText(QRectF(
                              (x - ld.x()) / delta.x() * 256.0f,
                              256 - (y - ld.y()) / delta.y() * 256.0f,
                              256, 256),
                          QString::fromStdString(mark.name));
    }
}


/**
 * @brief CMap2D::loadOGRData 加载单个.tab文件，读取到的矢量图元存入链表结构内
 * @param filePath  tab文件路径
 * @return 是否加载成功
 */
bool CMap2D::loadOGRData(QString filePath)
{
    OGRRegisterAll();

    OGRDataSource  *pOGRDataSource = OGRSFDriverRegistrar::Open( filePath.toStdString().c_str(), FALSE );
    if( pOGRDataSource == NULL)
    {
        printf( "Open failed.\n" );
        return FALSE;
    }

    int layerCount = pOGRDataSource->GetLayerCount();

    for(int layer = 0;layer < layerCount; layer++)
    {
        OGRLayer  *pOGRLayer = pOGRDataSource->GetLayer(layer);
        OGRFeature *pOGRFeature = NULL;

        OGRFeatureDefn *pOGRFeatureDefn = pOGRLayer->GetLayerDefn();
        OGRFieldDefn *pOGRFieldDefn = NULL;
        OGRFieldType fieldtype;

        OGRGeometry * pOGRGeometry;
        OGRPoint pOGRPoint;
        OGRLineString *pOGRLineString;
        OGRLinearRing *pOGRLinearRing;
        OGRPolygon * pOGRPolygon ;
        pOGRLayer->ResetReading();
        while( (pOGRFeature = pOGRLayer->GetNextFeature()) != NULL )
        {
            //获取名称
            pOGRFieldDefn = pOGRFeatureDefn->GetFieldDefn(0);
            fieldtype = pOGRFieldDefn->GetType();
            std::string fieldName;
            QString tt;
            if (fieldtype == OFTString)
            {
                const char* tempName = pOGRFeature->GetFieldAsString(0);
                fieldName = QTextCodec::codecForName("GBK")->toUnicode(tempName).toStdString();
            }

            pOGRGeometry = pOGRFeature->GetGeometryRef(); //获取该要素的几何形状
            if(pOGRGeometry)
            {
                int nMultiCount = 1; //几何集中几何形状的个数
                switch(wkbFlatten(pOGRGeometry->getGeometryType()))
                {
                case wkbPoint://点
                {
                    OGRPoint *pPoint = (OGRPoint*) pOGRGeometry;

                    MapPoint ptempPoint = WebMercatorFromOGRPoint(*pPoint);
                    points.push_back(ptempPoint);  //point add

                    //加载汉字
                    if (!fieldName.empty())
                    {
                        MapMark ptempMark;
                        ptempMark.position = WebMercatorFromOGRPoint(*pPoint);
                        ptempMark.name = fieldName;
                        marks.push_back(ptempMark);    //mark add
                    }


                    break;
                }
                case wkbLineString:	//线
                {
                    pOGRLineString = (OGRLineString*)pOGRGeometry;
                    int numPoint = pOGRLineString->getNumPoints();

                    for (int i=0;i < numPoint;i++)
                    {
                        pOGRLineString->getPoint(i,&pOGRPoint);
                        MapPoint mp = WebMercatorFromOGRPoint(pOGRPoint);
                        lines.push_back(mp);    // line add
                        if(i != 0)lines.push_back(mp);
                    }
                    lines.push_back(lines.back());

                    //加载汉字
                    //if (!fieldName.empty())
                    //{
                    //    MapMark* ptempMark = new MapMark;
                    //    ptempMark->position = new MapPoint;
                    //    ptempMark->position->x = vXYZ.x;
                    //    ptempMark->position->y = vXYZ.y;
                    //    ptempMark->position->z = vXYZ.z;
                    //    ptempMark->name = fieldName;
                    //    CSingleMap* pTemp = new CSingleMap();
                    //    pTemp->type = 3;
                    //    pTemp->pMark = ptempMark;
                    //    push(pTemp);
                    //}
                    break;
                }
                case wkbPolygon://多边形
                {
                    pOGRPolygon = (OGRPolygon*)pOGRGeometry;
                    pOGRLinearRing = pOGRPolygon->getExteriorRing();
                    int numExLine = pOGRLinearRing->getNumPoints();    // 外环点的个数
                    int numInLine = pOGRPolygon->getNumInteriorRings();// 内环个数

                    for (int i=0;i < numExLine;i ++)
                    {
                        pOGRLinearRing->getPoint(i,&pOGRPoint);
                        MapPoint mp = WebMercatorFromOGRPoint(pOGRPoint);

                        lines.push_back(mp);    //line add
                        if(i != 0)lines.push_back(mp);
                    }
                    lines.push_back(lines.back());

                    // 存储内环
                    for (int j=0;j < numInLine;j ++)
                    {
                        pOGRLinearRing = pOGRPolygon->getInteriorRing(j);

                        //   每个内环的点的个数
                        int nInPointCount = pOGRLinearRing->getNumPoints();
                        for (int k=0; k < nInPointCount; k ++)
                        {
                            pOGRLinearRing->getPoint(k,&pOGRPoint);
                            MapPoint mp = WebMercatorFromOGRPoint(pOGRPoint);

                            lines.push_back(mp);    //line add
                            if(k != 0)lines.push_back(mp);
                        }
                        lines.push_back(lines.back());
                    }
                    //加载汉字
                    //if (!fieldName.empty())
                    //{
                    //    MapMark* ptempMark = new MapMark;
                    //    ptempMark->position = new MapPoint;
                    //    ptempMark->position->x = vXYZ.x;
                    //    ptempMark->position->y = vXYZ.y;
                    //    ptempMark->position->z = vXYZ.z;
                    //    ptempMark->name = fieldName;
                    //    CSingleMap* pTemp = new CSingleMap();
                    //    pTemp->type = 3;
                    //    pTemp->pMark = ptempMark;
                    //    push(pTemp);
                    //}
                    break;
                }
                case wkbMultiPoint:
                {
                    OGRMultiPoint *pOGRMultiPoint = (OGRMultiPoint*)pOGRGeometry;
                    nMultiCount = pOGRMultiPoint->getNumGeometries();
                    for (int i=0; i<nMultiCount; i++)
                    {
                        OGRPoint *pPoint = (OGRPoint*)pOGRMultiPoint->getGeometryRef(i);

                        MapPoint ptempPoint = WebMercatorFromOGRPoint(*pPoint);
                        points.push_back(ptempPoint); //point add
                    }
                }
                    break;
                case wkbMultiLineString:
                {

                    OGRMultiLineString *pOGRMultiLineString = ( OGRMultiLineString*)pOGRGeometry;
                    nMultiCount = pOGRMultiLineString->getNumGeometries();
                    for (int i=0; i<nMultiCount; i++)
                    {
                        pOGRLineString = (OGRLineString *)pOGRMultiLineString->getGeometryRef(i);
                        int numPoint = pOGRLineString->getNumPoints();

                        for (int i=0;i < numPoint;i++)
                        {
                            pOGRLineString->getPoint(i,&pOGRPoint);
                            MapPoint mp = WebMercatorFromOGRPoint(pOGRPoint);
                            lines.push_back(mp);    //line add
                            if(i != 0)lines.push_back(mp);
                        }
                        lines.push_back(lines.back());
                    }
                }
                    break;
                case wkbMultiPolygon:
                {
                    OGRMultiPolygon* pOGRMultiPolygon = (OGRMultiPolygon*)pOGRGeometry;
                    nMultiCount = pOGRMultiPolygon->getNumGeometries();
                    if(nMultiCount != 0)
                    {
                        for (int i=0; i<nMultiCount; i++)
                        {
                            pOGRPolygon = (OGRPolygon *)pOGRMultiPolygon->getGeometryRef(i);
                            pOGRLinearRing = pOGRPolygon->getExteriorRing();
                            int numExLine = pOGRLinearRing->getNumPoints();    // 外环点的个数
                            int numInLine = pOGRPolygon->getNumInteriorRings();// 内环个数

                            for (int i=0;i < numExLine;i ++)
                            {
                                pOGRLinearRing->getPoint(i,&pOGRPoint);
                                MapPoint mp = WebMercatorFromOGRPoint(pOGRPoint);
                                lines.push_back(mp);    //line add
                                if(i != 0)lines.push_back(mp);
                            }
                            lines.push_back(lines.back());

                            // 存储内环
                            for (int j=0;j < numInLine;j ++)
                            {
                                pOGRLinearRing = pOGRPolygon->getInteriorRing(j);

                                //   每个内环的点的个数
                                int nInPointCount = pOGRLinearRing->getNumPoints();
                                for (int k=0; k < nInPointCount; k ++)
                                {
                                    pOGRLinearRing->getPoint(k,&pOGRPoint);
                                    MapPoint mp = WebMercatorFromOGRPoint(pOGRPoint);
                                    lines.push_back(mp);    //line add
                                    if(k != 0)lines.push_back(mp);
                                }
                                lines.push_back(lines.back());
                            }
                        }
                    }
                }
                    break;
                case wkbGeometryCollection:
                    break;
                default:break;
                }//switch
            }
            OGRFeature::DestroyFeature( pOGRFeature );
        }//while
    }//for
    OGRDataSource::DestroyDataSource( pOGRDataSource );
    return TRUE;
}

/**
* @brief WebMercatorFromOGRPoint 获取到矢量地图内的点（地理坐标表示）以后，将点坐标转换为WebMercator坐标
* @param point
* @return
*/
MapPoint CMap2D::WebMercatorFromOGRPoint(const OGRPoint &point)
{
    double _MAX = MapConst::MAX;

    MapPoint p;

    p.x = point.getX() * _MAX / 180;
    p.y = std::log(std::tan((90 + point.getY()) * MapConst::PI / 360)) / (MapConst::PI / 180);
    p.y = p.y * _MAX / 180;

    makeBound(p);

    return p;
}

void CMap2D::makeBound(const MapPoint &point)
{
    if(point.x < minPoint.x) minPoint.x = point.x;
    if(point.y < minPoint.y) minPoint.y = point.y;
    if(point.x > maxPoint.x) maxPoint.x = point.x;
    if(point.y > maxPoint.y) maxPoint.y = point.y;
}

/**
 * @brief CMap2D::createVertexBuffer 使用地图加载时读如的数据初始化点、线VBO，使用VBO方式绘制地图，极大提升绘制速度
 */
void CMap2D::createVertexBuffer()
{
    funcs->glGenBuffers(1, &VBO_Points);
    funcs->glBindBuffer(GL_ARRAY_BUFFER, VBO_Points);
    funcs->glBufferData(GL_ARRAY_BUFFER, sizeof(MapPoint) * points.size(), &points[0], GL_STATIC_DRAW);

    funcs->glGenBuffers(1, &VBO_Lines);
    funcs->glBindBuffer(GL_ARRAY_BUFFER, VBO_Lines);
    funcs->glBufferData(GL_ARRAY_BUFFER, sizeof(MapPoint) * lines.size(), &lines[0], GL_STATIC_DRAW);

    funcs->glBindBuffer(GL_ARRAY_BUFFER, 0);

    // 创建VBO完成后可释放点、线数据
    lineNum = lines.size();
    pointNum = points.size();
//    points.clear();
//    lines.clear();
}
