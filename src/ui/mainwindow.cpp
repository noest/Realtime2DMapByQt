#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "config.h"
#include "floatsettingview.h"
#include "scenerender.h"
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    mFloatSetting = NULL;

    initRender();

    initConnect();
}


MainWindow::~MainWindow()
{
    delete ui;
    delete mSceneRender;
    delete mFloatSetting;
}

void MainWindow::initRender()
{
    mSceneRender = new SceneRender(this);
    ui->sceneLayout->addWidget(mSceneRender);
    mSceneRender->setMouseTracking(true);
}

void MainWindow::initConnect()
{
    connect(ui->actionExit, SIGNAL(triggered()), this, SLOT(exit()));
    connect(ui->actionFloatSetting, SIGNAL(triggered(bool)), this, SLOT(showFloatSettingView(bool)));
}

void MainWindow::exit()
{
    //点击退出按钮确认提示
    QMessageBox box(QMessageBox::Question,tr("提示"),tr("确定要退出程序?"), QMessageBox::Yes | QMessageBox::Cancel, this);
    box.setStandardButtons(QMessageBox::Yes | QMessageBox::Cancel);
    box.setButtonText(QMessageBox::Yes, tr("退出"));
    box.setButtonText(QMessageBox::Cancel, tr("取消"));

    int reply = box.exec();

    if(reply == QMessageBox::Yes)
    {
        close();
    }
}

/**
 * @brief MainWindow::showFloatSettingView 悬浮的那个按钮是否显示
 */
void MainWindow::showFloatSettingView(bool show)
{
    mFloatSetting->setVisible(show);
    Config::setValue("window/floatview", show);
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    if(mFloatSetting == NULL){
        mFloatSetting = new FloatSettingView(ui->centralWidget);
        mFloatSetting->setGeometry(ui->menuBar->pos().x() + 10, ui->menuBar->pos().y() + 10, 300, 23);
        mFloatSetting->setVisible(Config::getValue("window/floatview", true).toBool());
    }
}
