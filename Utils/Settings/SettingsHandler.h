#pragma once

#include <QObject>
#include <tuple>
#include <QSettings>
#include "../GamepadDriver/ZQGamepad.h"

#define ZSet SettingsHandler::getInstance()

class SettingsHandler  : public QObject
{
	Q_OBJECT

public:
	static SettingsHandler* getInstance();
public:
	SettingsHandler(QObject *parent=nullptr);
	~SettingsHandler();
	std::tuple<QString, uint16_t> getIPAndPort();
	std::tuple<QString, QString> getBackendPathAndName();
	bool isVIP();
	bool isBackendRemote();
	std::tuple<QString, QString> getRemoteBackendUserNameAndIP();
private:
	QVariant WRSettings(QString key, QVariant default_value);
	QSettings* settings__;
};
