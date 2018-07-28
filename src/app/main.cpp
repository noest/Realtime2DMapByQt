#include "src/ui/mainwindow.h"
#include <QApplication>
#include "config.h"
#include <QDebug>
#include <QPointF>
#include "maputil.h"
#include "vectorimagemaker.h"
#include <QStyleFactory>
#include "imageloadmanager.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QStringList styleList = QStyleFactory::keys();
    a.setStyle(QStyleFactory::create(styleList.at(3)));

    Config::getInstance()->init();

    MainWindow w;
    w.show();

    a.installEventFilter(&w);
    return a.exec();
}
