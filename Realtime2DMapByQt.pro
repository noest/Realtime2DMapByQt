#-------------------------------------------------
#
# Project created by QtCreator 2018-07-28T15:12:21
#
#-------------------------------------------------

QT       += core gui opengl

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Realtime2DMapByQt
TEMPLATE = app

DESTDIR = $$_PRO_FILE_PWD_/

CONFIG += C++11

QMAKE_CXXFLAGS += -fpermissive

LIBS += \
    -lopengl32 \
    $$_PRO_FILE_PWD_/Dependencies/gdal/lib/libgdal.dll

INCLUDEPATH += \
    src/app \
    src/scene/map/tilemap \
    src/scene/map/vectormap \
    src/util \
    src/scene \
    src/ui \
    src/scene/util

SOURCES += \
    src/app/main.cpp \
    src/ui/mainwindow.cpp \
    src/app/config.cpp \
    src/scene/fpsrender.cpp \
    src/scene/scenerender.cpp \
    src/scene/util/distutil.cpp \
    src/scene/map/tilemap/imageloadmanager.cpp \
    src/scene/map/tilemap/imagelrucache.cpp \
    src/scene/map/tilemap/maprender.cpp \
    src/scene/map/tilemap/maptile.cpp \
    src/scene/map/tilemap/tileinfo.cpp \
    src/scene/map/vectormap/map2d.cpp \
    src/scene/map/vectormap/vectorimagemaker.cpp \
    src/util/maputil.cpp \
    src/ui/floatsettingview.cpp

FORMS += \
    src/ui/mainwindow.ui \
    src/ui/floatsettigview.ui

HEADERS += \
    src/ui/mainwindow.h \
    src/app/config.h \
    src/scene/fpsrender.h \
    src/scene/irender.h \
    src/scene/scenerender.h \
    src/scene/util/distutil.h \
    src/scene/map/tilemap/imageloadmanager.h \
    src/scene/map/tilemap/imagelrucache.h \
    src/scene/map/tilemap/maprender.h \
    src/scene/map/tilemap/maptile.h \
    src/scene/map/tilemap/tileinfo.h \
    src/scene/map/vectormap/map2d.h \
    src/scene/map/vectormap/mapshapes.h \
    src/scene/map/vectormap/vectorimagemaker.h \
    src/util/cutil.h \
    src/util/lrucache.h \
    src/util/maputil.h \
    src/ui/floatsettingview.h
