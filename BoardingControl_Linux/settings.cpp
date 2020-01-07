#include "settings.h"

#include <QDir>
#include <QTextCodec>
#include <QMessageBox>
#include <QDebug>

QSettings *LocalSettings::config{nullptr};

LocalSettings::LocalSettings()
{
    QString filePath = QDir::currentPath() + "/config/settings.ini";
    QFileInfo configFile(filePath);

    if (!configFile.exists())
    {
       qCritical()<< QStringLiteral("file \"./config/settings.ini\" missing!");
    }

    config = new QSettings(filePath, QSettings::IniFormat);
    config->setIniCodec(QTextCodec::codecForName("GB2312"));//配置文件支持汉字
}

LocalSettings::~LocalSettings()
{
    if (config != nullptr){
        delete config;
    }
}


