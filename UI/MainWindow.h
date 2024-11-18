#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "ElaWindow.h"
#include "HomePage.h"
#include "ViewDataPage.h"
#include "PilotPage.h"
#include "AboutPage.h"
#include "SettingsPage.h"
#include "ElaDockWidget.h"
#include "VirtualTrackpad/VirtualTrackpad.h"
#include <QResizeEvent>
#include <QTimer>
#include "Communication/RCM/SessionManager.h"


class MainWindow : public ElaWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();


private:
    void InitWindow();
    void InitPage();
    void InitFooter();
    void InitSignalSlot();
    void resizeEvent(QResizeEvent* event);
    void changeEvent(QEvent* event);
    void InitMica();
    void InitDockVirtualTrackpad();
    void InitSSHConnection();
    bool isDarkMode();
    QString getMicaBackground();
private:
    QSystemTrayIcon* TrayIcon__;
    HomePage* HomePage__;
    ViewDataPage* ViewDataPage__;
    PilotPage* PilotPage__;
    SettingsPage* SettingsPage__;
    AboutPageCentralWidget* AboutWindow__;
    VirtualTrackpad* VirtualTrackpad__;
    ElaDockWidget* WindowBottomDocker__;
    QString AboutKey__;
    zzs::BITBOT_TCP_PROTOCAL_V1* CommHandle__;


	QTimer* CheckSSHConnectionTimer__;
	zzs::SessionManager* SessionManager__;
	bool Connected__;
	bool Errored__;
};
#endif // MAINWINDOW_H
