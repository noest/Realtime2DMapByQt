#include "config.h"
#include <QVariant>

SINGLETON_CPP(Config)

Config::~Config()
{
    delete mSetting;
}


void Config::setValue(QString key, QVariant value)
{
    getInstance()->mSetting->setValue(key, value);
}

QVariant Config::getValue(QString key)
{
    QVariant value = getInstance()->mSetting->value(key);
    return value;
}


QVariant Config::getValue(QString key, QVariant def)
{
    if(!getInstance()->mSetting->contains(key)){
        getInstance()->mSetting->setValue(key, def);
    }
    QVariant value = getInstance()->mSetting->value(key, def);
    return value;
}

void Config::init()
{
    mSetting = new QSettings("./config.ini", QSettings::IniFormat);
}
