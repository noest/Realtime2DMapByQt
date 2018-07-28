#include "floatsettingview.h"
#include "ui_floatsettigview.h"
#include "allhead.h"

FloatSettingView::FloatSettingView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FloatSettigView)
{
    ui->setupUi(this);

    inShow = Config::getValue("floatsetting/inshow", true).toBool();

    initConenct();

    updateStatus();
}


FloatSettingView::~FloatSettingView()
{
    delete ui;
}

void FloatSettingView::updateStatus()
{
    ui->btn_road->setVisible(inShow);
    ui->btn_boundary->setVisible(inShow);
    ui->btn_marksmp->setVisible(inShow);
    ui->btn_markfull->setVisible(inShow);

    ui->btn_showhide->setText(inShow ? tr("收起") : tr("展开"));

    ui->btn_road->setChecked(Config::getValue("vectormap/road", true).toBool());
    ui->btn_boundary->setChecked(Config::getValue("vectormap/boundary", true).toBool());
    ui->btn_marksmp->setChecked(Config::getValue("vectormap/marksmp", true).toBool());
    ui->btn_markfull->setChecked(Config::getValue("vectormap/markfull", true).toBool());
}

void FloatSettingView::initConenct()
{
    connect(ui->btn_showhide, SIGNAL(clicked()), this, SLOT(showOrHide()));

    connect(ui->btn_road, SIGNAL(clicked()), this, SLOT(vectorMapSet()));
    connect(ui->btn_boundary, SIGNAL(clicked()), this, SLOT(vectorMapSet()));
    connect(ui->btn_marksmp, SIGNAL(clicked()), this, SLOT(vectorMapSet()));
    connect(ui->btn_markfull, SIGNAL(clicked()), this, SLOT(vectorMapSet()));
}

void FloatSettingView::showOrHide()
{
    inShow = !inShow;
    Config::setValue("floatsetting/inshow", inShow);
    updateStatus();
}

void FloatSettingView::vectorMapSet()
{
    Config::setValue("vectormap/road", ui->btn_road->isChecked());
    Config::setValue("vectormap/boundary", ui->btn_boundary->isChecked());
    Config::setValue("vectormap/marksmp", ui->btn_marksmp->isChecked());
    Config::setValue("vectormap/markfull", ui->btn_markfull->isChecked());
}
