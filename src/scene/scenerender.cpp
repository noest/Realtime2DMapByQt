#include "scenerender.h"
#include <QDebug>
#include "timeutil.h"
#include <QPainter>
#include <QPaintEvent>
#include <QtAlgorithms>
#include "src/scene/irender.h"
#include "src/scene/fpsrender.h"
#include <QDesktopWidget>
#include <QApplication>
#include "maprender.h"
#include "config.h"
#include <QOpenGLBuffer>
#include <QImage>
#include <QVector2D>
#include "vectorimagemaker.h"
#include <QTimer>
#include <QMenu>
#include "distutil.h"

bool comp(const RenderInfo &a, const RenderInfo &b)
{
    return a.layer < b.layer;
}

SceneRender::SceneRender(QWidget *parent)
    : QOpenGLWidget(parent)
{
    initRender();
    startRend();
    initPopMenu();

}

SceneRender::~SceneRender()
{
    // TODO 删除FPS render
    mFrameTimer->stop();
    delete mFrameTimer;
}

void SceneRender::addRender(IRender *render, int layer)
{
    RenderInfo renderInfo;
    renderInfo.render = render;
    renderInfo.layer = layer;
    mRenderInfoList.push_back(renderInfo);
    qSort(mRenderInfoList.begin(), mRenderInfoList.end(), comp);
}

void SceneRender::animate()
{
    update();
}

void SceneRender::initRender()
{
    // FPS绘制
    addRender(new FpsRender, IRender::LAYER_FPS);

    // 地图绘制
    MapRender::getInstance()->init();
    addRender(MapRender::getInstance(), IRender::LAYER_MAP);

    // 距离计算工具
    DistUtil::getInstance()->init();
    addRender(DistUtil::getInstance(), IRender::LAYER_FPS);
}

void SceneRender::initializeGL()
{
    initializeOpenGLFunctions();
    mContext = context();
    VectorImageMaker::getInstance()->init(256, 256);
}

void SceneRender::paintEvent(QPaintEvent *event)
{
    makeCurrent();
    {
        renderFrame();
    }
    doneCurrent();
}

void SceneRender::resizeGL(int w, int h)
{
    Config::setValue("window/width", w);
    Config::setValue("window/height", h);
    foreach (RenderInfo renderInfo, mRenderInfoList) {
        renderInfo.render->windowSizeChange(w, h);
    }
}

void SceneRender::paintGL()
{

}

void SceneRender::renderFrame()
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.beginNativePainting();
    {
        glClearColor(0, 0, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT);
        foreach (RenderInfo renderInfo, mRenderInfoList) {
            renderInfo.render->drawFrame(&painter);
        }
    }
    painter.endNativePainting();

    foreach (RenderInfo renderInfo, mRenderInfoList) {
        renderInfo.render->afterDraw();
    }
}

void SceneRender::startRend()
{
    mFrameTimer = new QTimer(this);
    connect(mFrameTimer, SIGNAL(timeout()), this, SLOT(animate()));
    mFrameTimer->start(int(1000/60.0));
}

void SceneRender::initPopMenu()
{
    distCaculate = new QAction("距离计算", this);

    popMenu = new QMenu(this);//定义一个右键弹出菜单
    popMenu->addAction(distCaculate);

    connect(distCaculate, SIGNAL(triggered(bool)), DistUtil::getInstance(), SLOT(startRange()));
}

void SceneRender::mousePressEvent(QMouseEvent *event)
{
    QPointF mercator = MapRender::getInstance()->getMercatorFromWindowPos(event->pos());
    for(int i = mRenderInfoList.size() - 1 ; i >= 0 ; --i){
        RenderInfo renderInfo = mRenderInfoList.at(i);
        if(renderInfo.render->mousePressEvent(event, &mercator)){
            return;
        }
    }
}

void SceneRender::mouseReleaseEvent(QMouseEvent *event)
{
    bool handled = false;
    QPointF mercator = MapRender::getInstance()->getMercatorFromWindowPos(event->pos());
    for(int i = mRenderInfoList.size() - 1 ; i >= 0 ; --i){
        RenderInfo renderInfo = mRenderInfoList.at(i);
        if(renderInfo.render->mouseReleaseEvent(event, &mercator)){
            handled = true;
//            break;
        }
    }

    if(!handled && event->button() == Qt::RightButton){
        // 显示右键菜单
        popMenu->exec(QCursor::pos());//弹出右键菜单，菜单位置为光标位置
    }
}

void SceneRender::mouseMoveEvent(QMouseEvent *event)
{
    QPointF mercator = MapRender::getInstance()->getMercatorFromWindowPos(event->pos());
    for(int i = mRenderInfoList.size() - 1 ; i >= 0 ; --i){
        RenderInfo renderInfo = mRenderInfoList.at(i);
        if(renderInfo.render->mouseMoveEvent(event, &mercator)){
            break;
        }
    }
}

void SceneRender::wheelEvent(QWheelEvent *event)
{
    foreach (RenderInfo renderInfo, mRenderInfoList) {
        renderInfo.render->wheelEvent(event);
    }
}

