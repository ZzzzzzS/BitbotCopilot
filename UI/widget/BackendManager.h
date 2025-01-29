#pragma once

#include <QWidget>
#include "MetaRTDView.h"
#include "ElaTheme.h"
#include "Communication/MetaBackendCom.hpp"

QT_BEGIN_NAMESPACE
namespace Ui {
    class BackendManager;
}
QT_END_NAMESPACE

class BackendManager : public MetaRTDView
{
    Q_OBJECT

public:
    BackendManager(QWidget* parent = nullptr);
    ~BackendManager();
    void ResetUI();
    bool isRunning();
    bool StartBackend();
    void TerminateBackend();

private:
    void ThemeChanged(ElaThemeType::ThemeMode themeMode);
    void closeEvent(QCloseEvent* event);
    void ConnectionButtonClickedSlot();
private:
	zzs::MetaBackendCommander* CmdHandle;
    bool isRemote = false;
    QString ExecPath;
    QString ExecName;
    Ui::BackendManager* ui;
};
