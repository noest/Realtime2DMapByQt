#include "fpsrender.h"
#include <QTime>
#include <QPen>
#include <QBrush>
#include <QFont>
#include <QPainter>
#include <QRect>
#include <QDebug>
#include <QPaintEvent>
#include <QFontMetrics>
#include <QLabel>
#include "timeutil.h"
#include "maputil.h"
#include "maprender.h"

FpsRender::FpsRender()
{
    mTimeCounter = new QTime();
    mTimeCounter->start();
    mTextPen = QPen(Qt::white);
    mTextFont.setPixelSize(16);
    mBackground = QBrush(QColor(80, 80, 80));
}

FpsRender::~FpsRender()
{
    delete mTimeCounter;
}

void FpsRender::drawFrame(QPainter *painter)
{
    calculateFps();

    QString textToShow = QString("FPS:%1").arg(mFps);

//    painter->fillRect(event->rect(), mBackground);
    painter->setRenderHint(QPainter::Antialiasing);
    painter->setPen(mTextPen);
    painter->setFont(mTextFont);

    int width = painter->fontMetrics().width(textToShow);
    int height = painter->fontMetrics().height();

    QRect rect = calculatePosition(width, height);

    // 测试使用
//    QPointF newCenter = GeographManager::getInstance()->getWindowPosOfLgla(117.563304, 31.528802);
    QPointF mer = MapUtil::getMercatorFromLgLa(117.563304, 31.528802);
//    QMatrix matrix = MapRender::getInstance()->getMercatorToPosMatrix();
    QPointF newCenter = MapRender::getInstance()->getWindowPosFromMercator(mer);
//    qDebug()<<"calculate: "<< newCenter<<"   matrix:"<< mer * matrix;
    rect.moveCenter(QPoint((int)newCenter.x(), (int)newCenter.y()));

    painter->drawText(rect, Qt::AlignCenter, textToShow);

//    QPen p;
//    p.setWidth(0);
//    painter->setPen(p);
//    painter->setMatrix(MapRender::getInstance()->getMercatorToPosMatrix());
//    painter->drawLine(mer.x(), mer.y(), 0,0);
}

void FpsRender::calculateFps()
{
    mFrameRended++;
    if(mTimeCounter->elapsed() > TimeUtil::SECOND_MILLS){
        mFps = mFrameRended;
        mFrameRended = 0;
        mTimeCounter->restart();
    }
}

QRect FpsRender::calculatePosition(int width, int height)
{
    QPoint point(100,100);
    QRect rect;
    rect.setTop(point.y());
    rect.setBottom(point.y() + height);
    rect.setLeft(point.x()-width);
    rect.setRight(point.x());
    return rect;
}
