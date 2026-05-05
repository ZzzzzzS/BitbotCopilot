#pragma once

#include <QWidget>

struct RobotConfig_t;

class AvatarNameEditor;
class SettingsFrontEndWidget;
class SettingsBackEndWidget;
class SettingsDatabaseWidget;
class SettingsAutorunLauncher;

class RobotConfigForm : public QWidget
{
    Q_OBJECT
public:
    explicit RobotConfigForm(QWidget* parent = nullptr);

    void loadFromConfig(const RobotConfig_t& config);
    void saveToConfig(RobotConfig_t& config) const;

    QString profileName() const;
    void setProfileName(const QString& name);
    void setProfileNameEditable(bool editable);
    void setAutorunVisible(bool visible);

    QString avatarPath() const;
    void setAvatarPath(const QString& path);

private:
    AvatarNameEditor* avatarNameEditor__;
    SettingsFrontEndWidget* frontendSettings__;
    SettingsBackEndWidget* backendSettings__;
    SettingsDatabaseWidget* databaseSettings__;
    SettingsAutorunLauncher* autorunLauncher__;
};
