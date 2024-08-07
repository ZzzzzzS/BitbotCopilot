#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "ElaWindow.h"
#include "HomePage.h"
#include "ViewDataPage.h"
#include "PilotPage.h"
#include "AboutPage.h"
#include "SettingsPage.h"


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

private:
    HomePage* HomePage__;
    ViewDataPage* ViewDataPage__;
    PilotPage* PilotPage__;
    SettingsPage* SettingsPage__;
    AboutPage* AboutPage__;
};
#endif // MAINWINDOW_H
