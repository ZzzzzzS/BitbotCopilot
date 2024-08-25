#pragma once

#include <QWidget>
#include <QProcess>
#include "MetaRTDView.h"
#include "ElaTheme.h"

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

signals:
    void ProcessStarted();
    void ProcessFinished();
    void ProcessErrored();

private:
    void ThemeChanged(ElaThemeType::ThemeMode themeMode);
    void closeEvent(QCloseEvent* event);
    void TerminateBackend();
    void ConnectionButtonClickedSlot();
private:
    bool isRemote = false;
    QProcess* BackendProcess__;
    QString UserName;
    QString IP;
    QString ExecPath;
    QString ExecName;
    Ui::BackendManager* ui;
};
