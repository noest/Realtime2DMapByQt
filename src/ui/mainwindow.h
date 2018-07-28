#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class SceneRender;
class QTimer;
class FloatSettingView;

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void exit();
    void showFloatSettingView(bool show);

private:
    void initRender();
    void initConnect();

private:
    Ui::MainWindow *ui;
    SceneRender *mSceneRender;
    FloatSettingView *mFloatSetting;
    // QWidget interface
protected:
    void resizeEvent(QResizeEvent *event);
};

#endif // MAINWINDOW_H
