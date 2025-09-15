#pragma once
#include "QWidget"
#include "ElaText.h"
#include "ElaDrawerArea.h"
#include "ElaSpinBox.h"
#include "ElaLineEdit.h"

class SettingsFrontEndWidget : public QWidget
{
    Q_OBJECT
public:
    SettingsFrontEndWidget(QWidget* parent);
    ~SettingsFrontEndWidget();

    void setIP(QString ip);
    void setPort(uint16_t port);
    QString getIP();
    uint16_t getPort();

private:
    ElaDrawerArea* DrawArea__;
    ElaText* title__;

    ElaLineEdit* ipAddress__;
    ElaSpinBox* portBox__;

};
