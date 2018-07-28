#include "distutil.h"
#include <QDebug>
#include <QPainter>
#include <maprender.h>
#include "maputil.h"

SINGLETON_CPP(DistUtil);

#define POS(x) MapRender::getInstance()->getWindowPosFromMercator(x)

DistUtil::init()
{
    firstSeted = false;
    secondSeted = false;
    ranging = false;
    dist = -1;
    done = true;
    moved = false;
}

void DistUtil::drawFrame(QPainter *painter)
{
    if(done || !moved)return;

    float pointSize = 3;

    QPen p;
    p.setWidth(0);
    p.setColor(Qt::yellow);
    painter->setPen(p);
    painter->setBrush(Qt::yellow);

    QFont f;
    f.setPointSize(10);
    painter->setFont(f);


    // 第一个点和其经纬度
    QPointF flgla = MapUtil::getLgLaFromMercator(firstMer);
    painter->drawEllipse(POS(firstMer), pointSize, pointSize);
    painter->drawText(QRectF(POS(firstMer).x() + 5, POS(firstMer).y() - 12, 200, 40),
                      QString("%1, %2")
                      .arg(QString::number(flgla.x(),'f', 4))
                      .arg(QString::number(flgla.y(), 'f', 4)));

    if(firstSeted){
        // 第二个点和其经纬度，距离
        painter->drawEllipse(POS(secondMer), pointSize, pointSize);
        QPointF slgla = MapUtil::getLgLaFromMercator(secondMer);
        QString distText = QString("%1, %2 \n%3")
                .arg(QString::number(slgla.x(), 'f', 4))
                .arg(QString::number(slgla.y(), 'f', 4))
                .arg(dist > -1 ? QString("距离:%1m").arg((int)dist) : "");
        painter->drawText(QRectF(POS(secondMer).x() + 5, POS(secondMer).y() - 12, 200, 40), distText);

        // 连线
        painter->drawLine(POS(firstMer), POS(secondMer));
    }
}

bool DistUtil::mousePressEvent(QMouseEvent *event, QPointF *mercator)
{
   return false;
}

bool DistUtil::mouseReleaseEvent(QMouseEvent *event, QPointF *mercator)
{
    if(!ranging) return false;

    if(secondSeted && event->button() == Qt::RightButton){
        done = true;
        ranging = false;
        return true;
    }

    if(!firstSeted){
        firstMer = *mercator;
        firstSeted = true;
        secondMer = firstMer;
    }else{
        if(!secondSeted){
            secondMer = *mercator;
            secondSeted = true;
        }
    }
    return true;
}

bool DistUtil::mouseMoveEvent(QMouseEvent *event, QPointF *mercator)
{
    if(ranging){
        mouseMer = *mercator;
        if(firstSeted){
            if(!secondSeted){
                dist = MapUtil::getGeographyDist(
                            MapUtil::getLgLaFromMercator(firstMer), MapUtil::getLgLaFromMercator(mouseMer));
                secondMer = *mercator;
            }
        }else{
            firstMer = *mercator;
        }
        moved = true;
    }
    return false;
}

void DistUtil::startRange()
{
    firstSeted = false;
    secondSeted = false;
    ranging = true;
    dist = -1;
    done = false;
    moved = false;
}
