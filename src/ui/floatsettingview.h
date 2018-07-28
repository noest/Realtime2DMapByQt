#ifndef FLOATSETTINGVIEW_H
#define FLOATSETTINGVIEW_H

#include <QWidget>

namespace Ui {
class FloatSettigView;
}

class FloatSettingView : public QWidget
{
    Q_OBJECT

public:
    explicit FloatSettingView(QWidget *parent = 0);
    ~FloatSettingView();
private:
    void updateStatus();
    void initConenct();
private slots:
    void showOrHide();
    // 矢量地图显示设置
    void vectorMapSet();

private:
    Ui::FloatSettigView *ui;
    bool inShow;
};

#endif // FLOATSETTINGVIEW_H
