#pragma once
#include "QWidget"
#include "ElaText.h"
#include "ElaDrawerArea.h"
#include "ElaCheckBox.h"
#include "ElaComboBox.h"
#include "ElaLineEdit.h"
#include "ElaPushButton.h"
#include "ElaToggleSwitch.h"

class SettingsDatabaseWidget : public QWidget
{
    Q_OBJECT
public:
    SettingsDatabaseWidget(QWidget* parent);
    ~SettingsDatabaseWidget();

    void setRemoteDataPath(QString path);
    void setLocalCachePath(QString path);
    void setCacheData(bool cache);
    QString getRemoteDataPath();
    QString getLocalCachePath();
    bool getCacheData();

private:
    ElaDrawerArea* DrawArea__;
    ElaText* title__;

    ElaLineEdit* remoteDataPath__;
    ElaLineEdit* localCachePath__;
    ElaToggleSwitch* cacheDataSwitch__;
    ElaPushButton* selectLocalCachePathButton__;

};
