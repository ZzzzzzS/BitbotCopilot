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

bool SettingsHandler::isVIP()
{
    return this->WRSettings("COMMON/VIP", true).toBool();
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

AutoRunCmdList SettingsHandler::getAutoRunCommandList()
{
    size_t cmd_num = this->settings__->beginReadArray("AUTORUNCOMMAND");
    if (cmd_num <= 0)
    {
        this->settings__->endArray();
        return AutoRunCmdList();
    }
        

    AutoRunCmdList cmd_list;
    for (size_t i = 0; i < cmd_num; i++)
    {
        this->settings__->setArrayIndex(i);
        AutoRunCommand_t t;
        t.KeyName = this->settings__->value("KEY_NAME", " ").toString();
        t.WaitTime = this->settings__->value("DURATION", 30000).toInt();
        t.WaitUntil = this->settings__->value("WAIT_UNTIL", "NAN").toString();
        cmd_list.push_back(t);
    }
    this->settings__->endArray();
    return cmd_list;

}

QVariant SettingsHandler::WRSettings(QString key, QVariant default_value)
{
    QVariant var = this->settings__->value(key, default_value);
    this->settings__->setValue(key, var);
    this->settings__->sync();
    return var;
}
