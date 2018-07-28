#ifndef IRENDER_H
#define IRENDER_H

#include <QBrush>
#include <QFont>
#include <QPen>
#include <QWidget>
#include <QMouseEvent>
#include <QWheelEvent>

class IRender : public QObject
{
    Q_OBJECT
public:
    /**
     * @brief drawFrame 所有的绘制对象需实现的接口
     * @param painter
     * @param event
     */
    virtual void drawFrame(QPainter *painter) = 0;
    virtual void afterDraw(){}
    /**
     * @brief mousePressEvent
     * @param event
     * @param mercator
     * @return 事件是否被消费 true:消费，不继续传递
     */
    virtual bool mousePressEvent(QMouseEvent *event, QPointF *mercator){return false;}
    virtual bool mouseReleaseEvent(QMouseEvent *event, QPointF *mercator){return false;}
    virtual bool mouseMoveEvent(QMouseEvent *event, QPointF *mercator){return false;}
    virtual void wheelEvent(QWheelEvent *event){}
    virtual void windowSizeChange(int w, int h){}

public:
    const static int LAYER_FPS = 10;            // 绘制对象的层级，FPS对应的层级
    const static int LAYER_MAP = 0;             // 绘制对象的层级，地图对应的层级
    const static int LAYER_VECTOR_MAP = 1;      // 绘制对象的层级，VectorMap对应的层级
    const static int LAYER_RADAR = 2;           // 绘制对象的层级，Radar层级
    const static int LAYER_BLIND_ABGLE = 3;     // 绘制对象的层级，遮蔽角层级
    const static int LAYER_TRACK = 4;           // 绘制对象的层级，轨迹层级
    const static int LAYER_TRUE_VALUE = 5;      // 绘制对象的层级，真值层级
};

#endif // RENDER_H
