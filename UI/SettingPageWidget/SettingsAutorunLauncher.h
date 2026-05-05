#pragma once
#include "QWidget"
#include "ElaScrollPageArea.h"
#include "ElaText.h"
#include "QPushButton"
#include "Utils/Settings/SettingsHandler.h"
#include "optional"

class SettingsAutorunLauncher : public QWidget
{
    Q_OBJECT
public:
    SettingsAutorunLauncher(QWidget* parent);
    ~SettingsAutorunLauncher();
    AutoRunCmdList getCmdList();
    void setCmdList(const AutoRunCmdList& list);

private:
    void OpenSettingsSlot();
private:
    ElaText* title__;
    ElaText* OpenSettings__;

    std::optional<AutoRunCmdList> cmdList__;
};