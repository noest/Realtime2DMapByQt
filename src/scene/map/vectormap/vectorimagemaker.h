#ifndef VECTORIMAGEMAKER_H
#define VECTORIMAGEMAKER_H

#include <GL/gl.h>
#include <tr1/functional>
#include <QOpenGLFunctions>
#include <QOpenGLContext>
#include <QImage>
#include <QSharedPointer>
#include <QOpenGLFramebufferObject>
#include <QOpenGLShaderProgram>
#include <QOpenGLPaintDevice>
#include "cutil.h"

class CMap2D;

class VectorImageMaker
{
    SINGLETON_H(VectorImageMaker)
public:
    bool init(int width, int height);
    QImage* makeImage(QImage* srcImg, int row, int col, int level);
    QList<CMap2D *> getMaps();
private:
    double windowWidth;
    double windowHeight;
    ~VectorImageMaker();
private:
    void drawVectorMap(QPointF ld, QPointF rt, int level);
    void drawVectorMarks(QPainter* painter, QPointF ld, QPointF rt, int level);
    void initVectorMap();

    QOpenGLPaintDevice* m_device;
    QOpenGLContext* m_context;
    QOpenGLFramebufferObject* m_fbo;
    QOpenGLFunctions* m_function;
    QOpenGLShaderProgram* m_program;

    QList<CMap2D *> maps;
};

#endif // VECTORIMAGEMAKER_H
