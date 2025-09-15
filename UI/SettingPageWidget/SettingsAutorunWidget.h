#pragma once
#include "QWidget"
#include "ElaScrollPageArea.h"
#include "ElaText.h"
#include "QPushButton"

class SettingsAutorunLauncher : public QWidget
{
    Q_OBJECT
public:
    SettingsAutorunLauncher(QWidget* parent);
    ~SettingsAutorunLauncher();
private:
    void OpenSettingsSlot();
private:
    ElaText* title__;
    ElaText* OpenSettings__;
};