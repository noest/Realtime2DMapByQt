#ifndef FPSRENDER_H
#define FPSRENDER_H

#include "irender.h"


class FpsRender : public IRender
{
public:
    FpsRender();
    ~FpsRender();

    // IRender interface
public:
    void drawFrame(QPainter *painter);

private:
    /**
     * @brief calculateFps 统计已经绘制的帧数，计算当前的FPS
     */
    void calculateFps();
    /**
     * @brief calculatePosition 根据传入的PaintEvent计算FPS绘制位置（右上角）
     * @param event
     * @param width 要显示文字的宽度
     * @param height 要显示文字的高度
     * @return
     */
    QRect calculatePosition(int width, int height);

private:
    QTime *mTimeCounter;
    int mFrameRended = 0;
    int mFps = 0;

    QPen mTextPen;
    QFont mTextFont;
    QBrush mBackground;
};

#endif // FPSRENDER_H
