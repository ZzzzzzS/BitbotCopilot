#include "SettingsHandler.h"
#include <QMutex>
#include <QMutexLocker>
#include <QSettings>
#include <QString>
#include <QList>
#include <QDir>
#include <QSet>

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
    QDir profile_dir("./profile");
    if (!profile_dir.exists())
        profile_dir.mkdir("./");

    QFile default_user_profile("./settings.ini");
    bool compatmode = false;
    if (default_user_profile.exists())
    {
        bool rtn = default_user_profile.copy("./profile/default_user.ini");
        if (rtn)
        {
            default_user_profile.remove();
        }
        else
        {
            if (QFile profile2("./profile/default_user.ini"); profile2.exists())
            {
                default_user_profile.remove();
            }
            else
            {
                compatmode = true;
            }
        }
    }

    this->UserListSettings__ = new QSettings("./profile/common_settings.ini", QSettings::IniFormat, this);
    QString UserProfileName = this->WRSettings("CURRENT_USER_PROFILE", "default_user.ini", this->UserListSettings__).toString();
    if (UserProfileName.contains("common_settings.ini")) //防止注入
        UserProfileName = "default_user.ini";


    QStringList Profiles = this->getUserList();
    Profiles.append(UserProfileName);
    this->updateUserList(Profiles);

    UserProfileName = QString("./profile/") + UserProfileName;
    UserProfileName = compatmode ? QString("./settings.ini") : UserProfileName;
    this->settings__ = new QSettings(UserProfileName, QSettings::IniFormat, this);
    this->getUpdateChannel();

}

SettingsHandler::~SettingsHandler()
{
    this->settings__->sync();
    this->UserListSettings__->sync();
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

QString SettingsHandler::getUpdateChannel()
{
    QString channel = this->WRSettings("COMMON/UPDATECHANNEL", UPDATE_CHANNEL, this->UserListSettings__).toString();
    return channel;
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

QVariant SettingsHandler::WRSettings(QString key, QVariant default_value, QSettings* domain)
{
    QVariant var = domain->value(key, default_value);
    domain->setValue(key, var);
    domain->sync();
    return var;
}



QStringList SettingsHandler::getUserList()
{
    size_t num = this->UserListSettings__->beginReadArray("USER_PROFILES");
    if (num <= 0)
    {
        this->UserListSettings__->endArray();
        return QStringList();
    }

    QStringList rtn;
    for (size_t i = 0; i < num; i++)
    {
        this->UserListSettings__->setArrayIndex(i);
        QString profile_path = this->UserListSettings__->value("PROFILE_PATH").toString();
        rtn.append(profile_path);
    }
    this->UserListSettings__->endArray();
    return rtn;
}

bool SettingsHandler::updateUserList(const QStringList& users)
{
    QSet<QString> ProfileSet;
    for (auto user : users)
    {
        ProfileSet.insert(user);
    }

    this->UserListSettings__->remove("USER_PROFILES");
    this->UserListSettings__->beginWriteArray("USER_PROFILES", ProfileSet.size());
    size_t i = 0;
    for (auto user : ProfileSet)
    {
        this->UserListSettings__->setArrayIndex(i++);
        this->UserListSettings__->setValue("PROFILE_PATH", user);
    }
    this->UserListSettings__->endArray();
    this->UserListSettings__->sync();
    return true;
}

bool SettingsHandler::updateCurrentUserProfile(const QString& profile)
{
    if (profile.contains("common_settings")) //防止注入
        return false;
    this->UserListSettings__->setValue("CURRENT_USER_PROFILE", profile);

    QStringList Profiles = this->getUserList();
    Profiles.append(profile);
    this->updateUserList(Profiles);


    return true;
}
