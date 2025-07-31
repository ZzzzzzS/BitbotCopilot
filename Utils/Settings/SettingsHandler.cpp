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


SettingsHandler::SettingsHandler(QObject* parent)
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

std::tuple<QString, QString, QString, QString> SettingsHandler::getBackendConfig_ex()
{
    QString IP = this->WRSettings("BACKEND/IP", "127.0.0.1").toString();
    QString Port = this->WRSettings("BACKEND/PORT", 22).toString();
    QString UserName = this->WRSettings("BACKEND/USERNAME", "bitbot").toString();
    QString UserPasswd = this->WRSettings("BACKEND/PASSWD", "bitbot").toString();

    return std::make_tuple(IP, Port, UserName, UserPasswd);
}

QString SettingsHandler::getBackendDataRootPath()
{
    QString Path = this->WRSettings("BACKEND/DATAPATH", "/home").toString();
    return Path;
}

bool SettingsHandler::isVIP()
{
    return this->WRSettings("COMMON/VIP", true).toBool();
}

bool SettingsHandler::isBackendRemote()
{
    QString protocal = this->WRSettings("BACKEND/PROTOCAL", "local").toString();
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

bool SettingsHandler::isChachingRemoteData()
{
    bool is_chache = this->WRSettings("BACKEND/DATAVIEWERCACHE", true).toBool();
    return is_chache;
}

bool SettingsHandler::isUpdateBetaChannel()
{
    bool is_beta = this->WRSettings("COMMON/BETA_CHANNEL", false).toBool();
    return is_beta;
}

QString SettingsHandler::getLocalCachePath()
{
    QString Path = this->WRSettings("BACKEND/DATAVIEWERCACHEPATH", "./cache").toString();
    return Path;
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
    cmd_list.back().WaitTime = 1000;
    return cmd_list;

}

QVariant SettingsHandler::WRSettings(QString key, QVariant default_value)
{
    QVariant var = this->settings__->value(key, default_value);
    this->settings__->setValue(key, var);
    this->settings__->sync();
    return var;
}
