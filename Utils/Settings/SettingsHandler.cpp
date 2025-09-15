#include "SettingsHandler.h"
#include <QMutex>
#include <QMutexLocker>
#include <QSettings>
#include <QString>
#include <QList>
#include <QDir>
#include <QSet>
#include <QRandomGenerator>
#include <QRegularExpression>

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

QStringList SettingsHandler::getAvailableAvatars()
{
    QStringList avatarPaths;
    for (int i = 0; i < 10; i++)
    {
        avatarPaths.append(QString(":/avatar/robot_avatar/robot-%1.png").arg(i));
    }
    return avatarPaths;
}

QString SettingsHandler::getRandomAvatar()
{
    QStringList avatars = getAvailableAvatars();
    int index = QRandomGenerator::global()->bounded(avatars.size());
    return avatars.at(index);
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
        bool rtn = default_user_profile.copy("./profile/Unnamed_Robot.ini");
        if (rtn)
        {
            default_user_profile.remove();
        }
        else
        {
            if (QFile profile2("./profile/Unnamed_Robot.ini"); profile2.exists())
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
    QString UserProfileName = this->getCurrentProfileName();
    if (UserProfileName.contains("common_settings.ini")) //防止注入
        UserProfileName = "Unnamed_Robot.ini";


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

std::tuple<QString, uint16_t> SettingsHandler::getIPAndPort(QSettings* domain)
{
    if (domain == nullptr)
        domain = this->settings__;
    QString IP = this->WRSettings("FRONTEND/IP", "127.0.0.1", domain).toString();
    uint16_t port = this->WRSettings("FRONTEND/PORT", 12888, domain).toInt();
    return std::make_tuple(IP, port);
}

std::tuple<QString, QString> SettingsHandler::getBackendPathAndName(QSettings* domain)
{
    if (domain == nullptr)
        domain = this->settings__;
    QString Path = this->WRSettings("BACKEND/PATH", "/", domain).toString();
    QString Exec = this->WRSettings("BACKEND/EXEC", "main_app", domain).toString();
    return std::make_tuple(Path, Exec);
}

std::tuple<QString, uint16_t, QString, QString> SettingsHandler::getBackendConfig_ex(QSettings* domain)
{
    if (domain == nullptr)
        domain = this->settings__;

    if (!this->isBackendRemote(domain))
    {
        return std::make_tuple(QString(), uint16_t(), QString(), QString());
    }

    QString IP = this->WRSettings("BACKEND/IP", "127.0.0.1", domain).toString();
    uint16_t Port = this->WRSettings("BACKEND/PORT", 22, domain).toUInt();
    QString UserName = this->WRSettings("BACKEND/USERNAME", "bitbot", domain).toString();
    QString UserPasswd = this->WRSettings("BACKEND/PASSWD", "bitbot", domain).toString();

    return std::make_tuple(IP, Port, UserName, UserPasswd);
}

QString SettingsHandler::getBackendDataRootPath(QSettings* domain)
{
    if (domain == nullptr)
        domain = this->settings__;

    if (!this->isBackendRemote(domain))
        return QString();

    QString Path = this->WRSettings("BACKEND/DATAPATH", "/home", domain).toString();
    return Path;
}

bool SettingsHandler::isBackendRemote(QSettings* domain)
{
    QString protocal;
    if (domain == nullptr)
        protocal = this->WRSettings("BACKEND/PROTOCAL", "local", this->settings__).toString();
    else
        protocal = this->WRSettings("BACKEND/PROTOCAL", "local", domain).toString();
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

bool SettingsHandler::isChachingRemoteData(QSettings* domain)
{
    if (domain == nullptr)
        domain = this->settings__;

    if (!this->isBackendRemote(domain))
    {
        return false; //如果不是远程后端，则不需要缓存
    }

    bool is_chache = this->WRSettings("BACKEND/DATAVIEWERCACHE", true, domain).toBool();
    return is_chache;
}

QString SettingsHandler::getUpdateChannel()
{
    QString channel = this->WRSettings("COMMON/UPDATECHANNEL", UPDATE_CHANNEL, this->UserListSettings__).toString();
    return channel;
}

QString SettingsHandler::getLocalCachePath(QSettings* domain)
{
    if (domain == nullptr)
        domain = this->settings__;

    if (!this->isBackendRemote())
    {
        return QString(); //如果不是远程后端，则不需要缓存路径
    }

    QString Path = this->WRSettings("BACKEND/DATAVIEWERCACHEPATH", "./cache", domain).toString();
    return Path;
}

QString SettingsHandler::getUserAvatarPath(QSettings* domain)
{
    if (domain == nullptr)
    {
        domain = this->settings__;
    }

    QString DefaultRandomAvatar = SettingsHandler::getRandomAvatar();

    QString Avatar = this->WRSettings("PROFILE/AVATAR", DefaultRandomAvatar, domain).toString();
    return Avatar;
}

//return Display_robotname, display_robot_ip, display_robot_avatar_path
std::tuple<QString, QString, QString> SettingsHandler::getUserProfileInfo()
{
    QString UserName = this->getCurrentProfileName();
    QString ip;
    if (this->isBackendRemote())
    {
        ip = this->WRSettings("BACKEND/IP", "127.0.0.1", this->settings__).toString();
    }
    else
    {
        ip = tr("local computer");
    }

    QString Avatar = this->getUserAvatarPath();

    return std::make_tuple(UserName, ip, Avatar);
}

QList<std::tuple<QString, QString, QString>> SettingsHandler::getUserProfileInfos(bool exclude_current_profile)
{
    QStringList users = this->getUserList(exclude_current_profile);
    QList<std::tuple<QString, QString, QString>> InfoList;

    for (auto&& user : users)
    {
        QString Profile_path = QString("./profile/") + user;
        QSettings setting = QSettings(Profile_path, QSettings::IniFormat, nullptr);

        QString ip;
        if (this->isBackendRemote(&setting))
        {
            ip = this->WRSettings("BACKEND/IP", "127.0.0.1", &setting).toString();
        }
        else
        {
            ip = tr("local computer");
        }

        QString Avatar = this->getUserAvatarPath(&setting);

        auto tuple = std::make_tuple(user, ip, Avatar);
        InfoList.append(tuple);
    }
    return InfoList;
}



AutoRunCmdList SettingsHandler::getAutoRunCommandList(QSettings* domain)
{
    if (domain == nullptr)
        domain = this->settings__;

    size_t cmd_num = domain->beginReadArray("AUTORUNCOMMAND");
    if (cmd_num <= 0)
    {
        domain->endArray();
        return AutoRunCmdList();
    }


    AutoRunCmdList cmd_list;
    for (size_t i = 0; i < cmd_num; i++)
    {
        domain->setArrayIndex(i);
        AutoRunCommand_t t;
        t.KeyName = domain->value("KEY_NAME", " ").toString();
        t.WaitTime = domain->value("DURATION", 30000).toInt();
        t.WaitUntil = domain->value("WAIT_UNTIL", "NAN").toString();
        cmd_list.push_back(t);
    }
    domain->endArray();
    cmd_list.back().WaitTime = 1000;
    return cmd_list;

}

bool SettingsHandler::GetAllConfig(RobotConfig_t& config, const QString& profilename, QString& error_msg)
{
    if (profilename.contains("common_settings")) //防止注入
    {
        error_msg = tr("Invalid profile name.");
        return false;
    }

    if (profilename.isEmpty())
    {
        error_msg = tr("Profile name is empty.");
        return false;
    }

    QString Current_Profile_Name = this->getCurrentProfileName();
    QSettings* domain;
    if (Current_Profile_Name == profilename)
        domain = this->settings__;
    else
        domain = new QSettings(QString("./profile/") + profilename, QSettings::IniFormat, this);

    config.Profile.Avatar = this->getUserAvatarPath(domain);
    auto [ip, port] = this->getIPAndPort(domain);
    config.Frontend.ip = ip;
    config.Frontend.port = port;

    auto [backend_path, backend_exec] = this->getBackendPathAndName(domain);
    config.Backend.Path = backend_path;
    config.Backend.Exec = backend_exec;
    auto [backend_ip, backend_port, backend_user, backend_passwd] = this->getBackendConfig_ex(domain);
    config.Backend.ip = backend_ip;
    config.Backend.Port = backend_port;
    config.Backend.Username = backend_user;
    config.Backend.Passwd = backend_passwd;
    config.Backend.DataPath = this->getBackendDataRootPath(domain);
    config.Backend.isRemote = this->isBackendRemote(domain);
    config.Backend.CacheRemoteData = this->isChachingRemoteData(domain);
    config.Backend.DataViewerCachePath = this->getLocalCachePath(domain);
    config.AutoRunCommands = this->getAutoRunCommandList(domain);
    if (domain != this->settings__)
    {
        delete domain;
    }
    return true;
}

bool SettingsHandler::SaveAllConfig(const RobotConfig_t& config, const QString& profilename, QString& error_msg)
{
    if (profilename.contains("common_settings")) //防止注入
    {
        error_msg = tr("Invalid profile name.");
        return false;
    }

    // check if profile name contains invalid characters for file names
    QRegularExpression invalidChars("[\\\\/:*?\"<>|]");
    if (profilename.contains(invalidChars))
    {
        error_msg = tr("Profile name contains invalid characters: \\ / : * ? \" < > |");
        return false;
    }

    if (profilename.isEmpty())
    {
        error_msg = tr("Profile name is empty.");
        return false;
    }

    if (config.Frontend.ip.isEmpty())
    {
        error_msg = tr("Frontend IP address is empty, please indicate a valid IP address.");
        return false;
    }

    if (config.Frontend.port == 0)
    {
        error_msg = tr("Frontend port is invalid, please indicate a valid port number (1-65535).");
        return false;
    }

    if (!config.Backend.ip.isEmpty())
    {
        if (config.Backend.Port == 0)
        {
            error_msg = tr("Backend port is invalid, please indicate a valid port number (1-65535).");
            return false;
        }

        if (config.Backend.ip == config.Frontend.ip && config.Backend.Port == config.Frontend.port)
        {
            error_msg = tr("Backend address and port cannot be the same as Frontend.");
            return false;
        }
    }

    QString Current_Profile_Name = this->getCurrentProfileName();
    QSettings* domain;
    if (Current_Profile_Name == profilename)
        domain = this->settings__;
    else
        domain = new QSettings(QString("./profile/") + profilename, QSettings::IniFormat, this);

    domain->setValue("PROFILE/AVATAR", config.Profile.Avatar);

    domain->setValue("FRONTEND/IP", config.Frontend.ip);
    domain->setValue("FRONTEND/PORT", config.Frontend.port);

    if (!config.Backend.Path.isEmpty())
        domain->setValue("BACKEND/PATH", config.Backend.Path);
    if (!config.Backend.Exec.isEmpty())
        domain->setValue("BACKEND/EXEC", config.Backend.Exec);

    if (config.Backend.isRemote)
    {
        domain->setValue("BACKEND/PROTOCAL", "ssh");
        domain->setValue("BACKEND/IP", config.Backend.ip);
        domain->setValue("BACKEND/PORT", config.Backend.Port);
        domain->setValue("BACKEND/USERNAME", config.Backend.Username);
        domain->setValue("BACKEND/PASSWD", config.Backend.Passwd);
        domain->setValue("BACKEND/DATAPATH", config.Backend.DataPath);
        domain->setValue("BACKEND/DATAVIEWERCACHEPATH", config.Backend.DataViewerCachePath);
        domain->setValue("BACKEND/DATAVIEWERCACHE", config.Backend.CacheRemoteData);
    }
    else
    {
        domain->setValue("BACKEND/PROTOCAL", "local");
    }



    // AutoRun Commands
    size_t cmd_num = config.AutoRunCommands.size();
    if (cmd_num != 0)
    {
        domain->remove("AUTORUNCOMMAND");
        domain->beginWriteArray("AUTORUNCOMMAND", cmd_num);
        for (size_t i = 0; i < cmd_num; i++)
        {
            const AutoRunCommand_t& t = config.AutoRunCommands[i];
            domain->setArrayIndex(i);
            domain->setValue("KEY_NAME", t.KeyName);
            domain->setValue("WAIT_UNTIL", t.WaitUntil);
            domain->setValue("DURATION", t.WaitTime);
        }
        domain->endArray();
    }

    domain->sync();

    if (domain != this->settings__)
        delete domain;

    return true;
}

QString SettingsHandler::getCurrentProfileName()
{
    return this->WRSettings("CURRENT_PROFILE/CURRENT_USER_PROFILE", "Unnamed_Robot.ini", this->UserListSettings__).toString();
}


QVariant SettingsHandler::WRSettings(QString key, QVariant default_value, QSettings* domain)
{
    QVariant var = domain->value(key, default_value);
    domain->setValue(key, var);
    domain->sync();
    return var;
}



QStringList SettingsHandler::getUserList(bool exclude_current_profile)
{
    QString Current_Profile_Name = this->getCurrentProfileName();

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
        if (exclude_current_profile && profile_path == Current_Profile_Name)
        {
            continue;
        }

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
    if (profile.isEmpty())
        return false;

    if (profile.contains("common_settings")) //防止注入
        return false;
    this->UserListSettings__->setValue("CURRENT_PROFILE/CURRENT_USER_PROFILE", profile);

    QStringList Profiles = this->getUserList();
    Profiles.append(profile);
    this->updateUserList(Profiles);

    this->UserListSettings__->sync();

    return true;
}

bool SettingsHandler::setUpdateChannel(const QString& url)
{
    this->UserListSettings__->setValue("COMMON/UPDATECHANNEL", url);
    this->UserListSettings__->sync();
    return true;
}
