#pragma once

#include <QObject>
#include <tuple>
#include <QSettings>
#include <QList>
#include "../GamepadDriver/ZQGamepad.h"

#define ZSet SettingsHandler::getInstance()

struct AutoRunCommand_t
{
	QString KeyName;
	QString WaitUntil;
	size_t WaitTime;
};

using AutoRunCmdList = QList<AutoRunCommand_t>;

struct RobotConfig_t
{
	struct {
		QString Avatar;
	} Profile;

	struct {
		QString ip;
		uint16_t port;
	} Frontend;

	struct {
		QString ip;
		uint16_t Port;
		QString Username;
		QString Passwd;
		QString Path;
		QString Exec;
		QString DataPath;
		bool isRemote;
		bool CacheRemoteData;
		QString DataViewerCachePath;
	} Backend;

	AutoRunCmdList AutoRunCommands;
};

class SettingsHandler : public QObject
{
	Q_OBJECT

public:
	static SettingsHandler* getInstance();
	static QStringList getAvailableAvatars();
	static QString getRandomAvatar();
public:
	SettingsHandler(QObject* parent = nullptr);
	~SettingsHandler();

	std::tuple<QString, uint16_t> getIPAndPort(QSettings* domain = nullptr);
	std::tuple<QString, QString> getBackendPathAndName(QSettings* domain = nullptr);
	std::tuple<QString, uint16_t, QString, QString> getBackendConfig_ex(QSettings* domain = nullptr);
	QString getBackendDataRootPath(QSettings* domain = nullptr);
	bool isBackendRemote(QSettings* domain = nullptr);
	bool isChachingRemoteData(QSettings* domain = nullptr);
	QString getLocalCachePath(QSettings* domain = nullptr);
	QString getUserAvatarPath(QSettings* domain = nullptr);

	AutoRunCmdList  getAutoRunCommandList(QSettings* domain = nullptr);

	QStringList getUserList(bool exclude_current_profile = false);
	std::tuple<QString, QString, QString> getUserProfileInfo();
	QList<std::tuple<QString, QString, QString>> getUserProfileInfos(bool exclude_current_profile = false);
	QString getUpdateChannel();
	bool updateUserList(const QStringList& profilesPath);
	bool updateCurrentUserProfile(const QString& profile);
	bool setUpdateChannel(const QString& url);

	bool GetAllConfig(RobotConfig_t& config, const QString& profilename, QString& error_msg);
	bool SaveAllConfig(const RobotConfig_t& config, const QString& profilename, QString& error_msg);
	QString getCurrentProfileName();

private:
	QVariant WRSettings(QString key, QVariant default_value, QSettings* domain);
	QSettings* settings__;
	QSettings* UserListSettings__;
};
