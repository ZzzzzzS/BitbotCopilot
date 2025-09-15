#pragma once

#include "ElaWidget.h"
#include "ElaDrawerArea.h"
#include "SettingsFrontEndWidget.h"
#include "SettingsBackendWidget.h"
#include "SettingsDatabaseWidget.h"
#include "SettingsAutorunWidget.h"
#include "ElaPushButton.h"
#include "AvatarNameEditor.h"

class PopupSettingsPage : public ElaWidget
{
    Q_OBJECT
public:
    PopupSettingsPage(QWidget* parent = nullptr);
    ~PopupSettingsPage();

    QString exec();

signals:
    void NewProfileCreated(QString ProfileName);

private:
    void InitBottomButton();
    void paintEvent(QPaintEvent* event) override;

private: //slots
    void onConfirmClicked();

private:
    AvatarNameEditor* AvatarNameEditor__;
    SettingsFrontEndWidget* FrontendSettings__;
    SettingsBackEndWidget* BackendSettings__;
    SettingsDatabaseWidget* DatabaseSettings__;
    SettingsAutorunLauncher* AutorunLaucher__;

    ElaPushButton* confirmButton__;
    ElaPushButton* cancelButton__;
};