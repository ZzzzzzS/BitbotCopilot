#include "RobotConfigForm.h"
#include "AvatarNameEditor.h"
#include "SettingsFrontEndWidget.h"
#include "SettingsBackendWidget.h"
#include "SettingsDatabaseWidget.h"
#include "SettingsAutorunLauncher.h"
#include "Utils/Settings/SettingsHandler.h"
#include "QVBoxLayout"

RobotConfigForm::RobotConfigForm(QWidget* parent)
    : QWidget(parent)
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    this->avatarNameEditor__ = new AvatarNameEditor(this);
    this->frontendSettings__ = new SettingsFrontEndWidget(this);
    this->backendSettings__ = new SettingsBackEndWidget(this);
    this->databaseSettings__ = new SettingsDatabaseWidget(this);
    this->autorunLauncher__ = new SettingsAutorunLauncher(this);

    this->setStyleSheet(R"(background-color:rgba(0,0,0,0);)");

    mainLayout->addWidget(this->avatarNameEditor__);
    mainLayout->addSpacerItem(new QSpacerItem(10, 10, QSizePolicy::Fixed, QSizePolicy::Fixed));
    mainLayout->addWidget(this->frontendSettings__);
    mainLayout->addSpacerItem(new QSpacerItem(10, 10, QSizePolicy::Fixed, QSizePolicy::Fixed));
    mainLayout->addWidget(this->backendSettings__);
    mainLayout->addSpacerItem(new QSpacerItem(10, 10, QSizePolicy::Fixed, QSizePolicy::Fixed));
    mainLayout->addWidget(this->databaseSettings__);
    mainLayout->addSpacerItem(new QSpacerItem(10, 10, QSizePolicy::Fixed, QSizePolicy::Fixed));
    mainLayout->addWidget(this->autorunLauncher__);
    mainLayout->addSpacerItem(new QSpacerItem(10, 10, QSizePolicy::Fixed, QSizePolicy::Fixed));
}

void RobotConfigForm::loadFromConfig(const RobotConfig_t& config)
{
    this->avatarNameEditor__->setAvatarPath(config.Profile.Avatar);
    this->frontendSettings__->setIP(config.Frontend.ip);
    this->frontendSettings__->setPort(config.Frontend.port);
    this->backendSettings__->setIP(config.Backend.ip);
    this->backendSettings__->setPort(config.Backend.Port);
    this->backendSettings__->setUsername(config.Backend.Username);
    this->backendSettings__->setUserPasswd(config.Backend.Passwd);
    this->backendSettings__->setExecPath(config.Backend.Path);
    this->backendSettings__->setExecName(config.Backend.Exec);
    this->backendSettings__->setIsRemote(config.Backend.isRemote);
    this->databaseSettings__->setRemoteDataPath(config.Backend.DataPath);
    this->databaseSettings__->setLocalCachePath(config.Backend.DataViewerCachePath);
    this->databaseSettings__->setCacheData(config.Backend.CacheRemoteData);
    this->autorunLauncher__->setCmdList(config.AutoRunCommands);
}

void RobotConfigForm::saveToConfig(RobotConfig_t& config) const
{
    config.Profile.Avatar = this->avatarPath();
    config.Frontend.ip = this->frontendSettings__->getIP();
    config.Frontend.port = this->frontendSettings__->getPort();
    config.Backend.ip = this->backendSettings__->getIP();
    config.Backend.Port = this->backendSettings__->getPort();
    config.Backend.Username = this->backendSettings__->getUsername();
    config.Backend.Passwd = this->backendSettings__->getUserPasswd();
    config.Backend.isRemote = this->backendSettings__->isCurrentRemoteProtocal();
    config.Backend.Path = this->backendSettings__->getExecPath();
    config.Backend.Exec = this->backendSettings__->getExecName();
    config.Backend.CacheRemoteData = this->databaseSettings__->getCacheData();
    config.Backend.DataViewerCachePath = this->databaseSettings__->getLocalCachePath();
    config.Backend.DataPath = this->databaseSettings__->getRemoteDataPath();
    config.AutoRunCommands = this->autorunLauncher__->getCmdList();
}

QString RobotConfigForm::profileName() const
{
    return this->avatarNameEditor__->getUserName();
}

void RobotConfigForm::setProfileName(const QString& name)
{
    this->avatarNameEditor__->setUserName(name);
}

void RobotConfigForm::setProfileNameEditable(bool editable)
{
    this->avatarNameEditor__->setNameEditable(editable);
}

void RobotConfigForm::setAutorunVisible(bool visible)
{
    this->autorunLauncher__->setVisible(visible);
}

QString RobotConfigForm::avatarPath() const
{
    return this->avatarNameEditor__->getAvatarPath();
}

void RobotConfigForm::setAvatarPath(const QString& path)
{
    this->avatarNameEditor__->setAvatarPath(path);
}
