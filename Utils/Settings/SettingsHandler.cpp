#include "SettingsHandler.h"
#include <QMutex>
#include <QMutexLocker>
#include <QSettings>
#include <QString>
#include <QList>

SettingsHandler* SettingsHandler::getInstance()
{
    static QMutex mutex;
    QMutexLocker locker(&mutex);
    static SettingsHandler* instance = nullptr;
    if (instance == nullptr)
    {
        instance = new SettingsHandler();
    }
    return instance;
}


SettingsHandler::SettingsHandler(QObject *parent)
	: QObject(parent)
{
    this->settings__ = new QSettings("./settings.ini", QSettings::IniFormat, this);
}

SettingsHandler::~SettingsHandler()
{
    this->settings__->sync();
}

std::tuple<QString, uint16_t> SettingsHandler::getIPAndPort()
{
    QString IP = this->WRSettings("FRONTEND/IP", "127.0.0.1").toString();
    uint16_t port = this->WRSettings("FRONTEND/PORT", 12888).toInt();
    return std::make_tuple(IP, port);
}

std::tuple<QString, QString> SettingsHandler::getBackendPathAndName()
{
    QString Path = this->WRSettings("BACKEND/PATH", "/").toString();
    QString Exec = this->WRSettings("BACKEND/EXEC", "main_app").toString();
    return std::make_tuple(Path, Exec);
}

bool SettingsHandler::isBackendRemote()
{
    QString protocal = this->WRSettings("BACKEND/PROTOCAL", "ssh").toString();
    if (protocal == QString("ssh"))
    {
        return true;
    }
    else if (protocal == QString("local"))
    {
        return false;
    }
    else
    {
        return false;
    }
}

std::tuple<QString, QString> SettingsHandler::getRemoteBackendUserNameAndIP()
{
    QString IP = this->WRSettings("BACKEND/IP", "127.0.0.1").toString();
    QString UserName = this->WRSettings("BACKEND/USERNAME", "bitbot").toString();
    return std::make_tuple(UserName, IP);
}

QVariant SettingsHandler::WRSettings(QString key, QVariant default_value)
{
    QVariant var = this->settings__->value(key, default_value);
    this->settings__->setValue(key, var);
    this->settings__->sync();
    return var;
}
