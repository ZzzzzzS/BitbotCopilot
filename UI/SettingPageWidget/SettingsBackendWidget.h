#pragma once
#include "QWidget"
#include "ElaText.h"
#include "ElaDrawerArea.h"
#include "ElaSpinBox.h"
#include "ElaLineEdit.h"
#include "ElaCheckBox.h"
#include "ElaComboBox.h"
#include "ElaScrollPageArea.h"

class SettingsBackEndWidget : public QWidget
{
    Q_OBJECT
public:
    SettingsBackEndWidget(QWidget* parent);
    ~SettingsBackEndWidget();

    void setIP(QString ip);
    void setPort(uint16_t port);
    void setUsername(QString name);
    void setUserPasswd(QString Passwd);
    void setExecPath(QString path);
    void setExecName(QString name);
    void setRemoteProtocal(QString protocal);
    void setIsRemote(bool isRemote);

    QString getIP();
    uint16_t getPort();
    QString getUsername();
    QString getUserPasswd();
    QString getExecPath();
    QString getExecName();
    QString getRemoteProtocal();
    bool isCurrentRemoteProtocal();

signals:
    void connectionProtocalChanged(const QString&);

private:
    void InitDrawRemoteConn();
    void InitDrawExecPath();
    void InitProtocalSelection();
private:
    ElaScrollPageArea* ProtocalSelectionArea__;
    ElaDrawerArea* DrawAreaRemoteConn__;
    ElaDrawerArea* DrawAreaExecPath__;

    ElaText* title__;

    ElaLineEdit* ipAddress__;
    ElaSpinBox* portBox__;
    ElaLineEdit* Username__;
    ElaLineEdit* Passwd__;
    ElaLineEdit* ExecPath__;
    ElaLineEdit* ExecName__;
    ElaComboBox* Protocal__;



};