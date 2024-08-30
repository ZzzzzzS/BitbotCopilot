#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "ElaWindow.h"
#include "HomePage.h"
#include "ViewDataPage.h"
#include "PilotPage.h"
#include "AboutPage.h"
#include "SettingsPage.h"
#include <QResizeEvent>


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

private:
    HomePage* HomePage__;
    ViewDataPage* ViewDataPage__;
    PilotPage* PilotPage__;
    SettingsPage* SettingsPage__;
    AboutPage* AboutPage__;
};
#endif // MAINWINDOW_H
