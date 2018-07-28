#ifndef DISTUTIL_H
#define DISTUTIL_H

#include "irender.h"
#include <QPointF>
#include "cutil.h"

class DistUtil : public IRender
{
    Q_OBJECT
    SINGLETON_H(DistUtil);
public:
    init();

public slots:
    void startRange();

    // IRender interface
public:
    void drawFrame(QPainter *painter);
    bool mousePressEvent(QMouseEvent *event, QPointF *mercator);
    bool mouseReleaseEvent(QMouseEvent *event, QPointF *mercator);
    bool mouseMoveEvent(QMouseEvent *event, QPointF *mercator);

private:
    QPointF firstMer;
    QPointF secondMer;
    QPointF mouseMer;
    qreal dist;

    bool firstSeted;
    bool secondSeted;
    bool ranging;
    bool done;
    bool moved;
};

#endif // DISTUTIL_H
