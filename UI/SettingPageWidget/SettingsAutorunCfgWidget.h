#pragma once
#include "ElaWidget.h"
#include "optional"
#include "Utils/Settings/SettingsHandler.h"
#include "QList"
#include "ElaText.h"
#include "ElaPushButton.h"
#include "QWidget"
#include "ClickableElaScrollPageArea.h"

class AutorunLineCfg : public QWidget
{
    Q_OBJECT
public:
    AutorunLineCfg(QWidget* parent);
    ~AutorunLineCfg();
    AutoRunCommand_t getLineCfg();
    void setLineCfg(AutoRunCommand_t& cfg);

};

class SettingsAutorunCfgWidget : public ElaWidget
{
    Q_OBJECT
public:
    SettingsAutorunCfgWidget(QWidget* parent = nullptr);
    ~SettingsAutorunCfgWidget();

    std::optional<AutoRunCmdList> getCmdList();

    std::optional<AutoRunCmdList> exec();

signals:
    void Selected();
    void Canceled();

private: //slots
    void ContinueButtonClickedSlot();
    void AddActionButtonClickedSlot();

private:
    void InitTitle();
    void abcdefg();
    void InitLayout();
private:
    QList<ClickableElaScrollPageArea*> ActionList__;


    ElaText* Title__;
    ElaText* Description__;
    ElaPushButton* CancelButton__;
    ElaPushButton* ContinueButton__;
    ElaPushButton* AddActionButton__;
};