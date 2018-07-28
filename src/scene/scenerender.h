#ifndef SCENERENDER_H
#define SCENERENDER_H

#include <QOpenGLWidget>
#include <QString>
#include <QTime>
#include <QPen>
#include <QBrush>
#include <QList>
#include <QOpenGLFunctions>
#include <QOpenGLContext>
#include <QImage>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLShaderProgram>
#include <QSharedPointer>
#include <QOpenGLBuffer>
#include <QAction>

class IRender;


struct RenderInfo{
    IRender *render;    // render
    int layer;          // 绘制所在的层，高的可遮挡低的
};

class SceneRender : public QOpenGLWidget, QOpenGLFunctions
{
    Q_OBJECT
public:
    SceneRender(QWidget *parent);
    ~SceneRender();

    /**
     * @brief addRender 添加一个render
     * @param render
     * @param layer render所在的层级，层级越高，越后绘制
     */
    void addRender(IRender* render, int layer);

public slots:
    void animate();

private:
    QVector<RenderInfo> mRenderInfoList;

private:
    void initRender();
    void renderFrame();
    void startRend();
    void initPopMenu();
    // QOpenGLWidget interface
protected:
    void paintEvent(QPaintEvent *event);
    void resizeGL(int w, int h);
    void initializeGL();
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);
    void paintGL();

    QOpenGLContext* mContext;
    QTimer *mFrameTimer;

    QAction* distCaculate;
    QMenu* popMenu;
};

#endif // MAPRENDER_H
