#include "SettingsBackendWidget.h"
#include "UI/widget/SettingsLineInfoWidget.h"
#include "QVBoxLayout"
#include "QHBoxLayout"

SettingsBackEndWidget::SettingsBackEndWidget(QWidget* parent)
    :QWidget(parent)
{
    this->title__ = new ElaText(this);
    this->title__->setTextStyle(ElaTextType::BodyStrong);
    this->title__->setText(tr("Backend manager"));
    this->title__->setTextPixelSize(18);
    this->title__->setAlignment(Qt::AlignTop);

    this->InitProtocalSelection();
    this->InitDrawRemoteConn();
    this->InitDrawExecPath();

    QVBoxLayout* MainLayout = new QVBoxLayout(this);
    MainLayout->addWidget(this->title__);
    MainLayout->addWidget(this->ProtocalSelectionArea__);
    MainLayout->addWidget(this->DrawAreaRemoteConn__);
    MainLayout->addWidget(this->DrawAreaExecPath__);
}

SettingsBackEndWidget::~SettingsBackEndWidget()
{

}

void SettingsBackEndWidget::setIP(QString ip)
{
    this->ipAddress__->setText(ip);
}

void SettingsBackEndWidget::setPort(uint16_t port)
{
    this->portBox__->setValue(port);
}

void SettingsBackEndWidget::setUsername(QString name)
{
    this->Username__->setText(name);
}

void SettingsBackEndWidget::setUserPasswd(QString Passwd)
{
    this->Passwd__->setText(Passwd);
}

void SettingsBackEndWidget::setExecPath(QString path)
{
    this->ExecPath__->setText(path);
}

void SettingsBackEndWidget::setExecName(QString name)
{
    this->ExecName__->setText(name);
}

void SettingsBackEndWidget::setRemoteProtocal(QString protocal)
{
    this->Protocal__->setCurrentText(protocal);
}

void SettingsBackEndWidget::setIsRemote(bool isRemote)
{
    this->Protocal__->setCurrentText(isRemote ? tr("ssh") : tr("local"));
}

QString SettingsBackEndWidget::getIP()
{
    return this->ipAddress__->text();
}

uint16_t SettingsBackEndWidget::getPort()
{
    return this->portBox__->value();
}

QString SettingsBackEndWidget::getUsername()
{
    return this->Username__->text();
}

QString SettingsBackEndWidget::getUserPasswd()
{
    return this->Passwd__->text();
}

QString SettingsBackEndWidget::getExecPath()
{
    return this->ExecPath__->text();
}

QString SettingsBackEndWidget::getExecName()
{
    return this->ExecName__->text();
}

QString SettingsBackEndWidget::getRemoteProtocal()
{
    return this->Protocal__->currentText();
}

bool SettingsBackEndWidget::isCurrentRemoteProtocal()
{
    return this->Protocal__->currentText() == tr("ssh");
}

void SettingsBackEndWidget::InitDrawRemoteConn()
{
    QWidget* DrawHead = new QWidget(this);
    SettingsLineInfoWidget* DrawHeadInfo = new SettingsLineInfoWidget(DrawHead, tr("Backend connection"), tr("Setup backend ip address, port, and login information"), QChar(ElaIconType::NetworkWired));
    QHBoxLayout* DrawHeadLayout = new QHBoxLayout(DrawHead);
    DrawHeadLayout->addWidget(DrawHeadInfo);
    DrawHeadLayout->addSpacerItem(new QSpacerItem(10, 10, QSizePolicy::Expanding, QSizePolicy::Fixed));

    QWidget* DrawIP = new QWidget(this);
    DrawIP->setFixedHeight(60);
    SettingsLineInfoWidget* DrawIPInfo = new SettingsLineInfoWidget(DrawIP, tr("Backend ip Address"));
    DrawIPInfo->setTitleFontSize(14);
    this->ipAddress__ = new ElaLineEdit(DrawIP);
    this->ipAddress__->setAlignment(Qt::AlignCenter);
    this->ipAddress__->setFixedSize(180, 30);
    this->ipAddress__->setPlaceholderText(QString("192.168.0.1"));
    QHBoxLayout* DrawIPLayout = new QHBoxLayout(DrawIP);
    DrawIPLayout->addWidget(DrawIPInfo);
    DrawIPLayout->addSpacerItem(new QSpacerItem(10, 10, QSizePolicy::Expanding, QSizePolicy::Fixed));
    DrawIPLayout->addWidget(this->ipAddress__);
    DrawIPLayout->addSpacerItem(new QSpacerItem(16, 10, QSizePolicy::Fixed, QSizePolicy::Fixed));

    QWidget* DrawPort = new QWidget(this);
    DrawPort->setFixedHeight(60);
    SettingsLineInfoWidget* DrawPortInfo = new SettingsLineInfoWidget(DrawPort, tr("Backend port"));
    DrawPortInfo->setTitleFontSize(14);
    this->portBox__ = new ElaSpinBox(DrawPort);
    this->portBox__->setRange(1, 65535);
    this->portBox__->setValue(22);
    this->portBox__->setFixedWidth(120);
    QHBoxLayout* DrawPortLayout = new QHBoxLayout(DrawPort);
    DrawPortLayout->addWidget(DrawPortInfo);
    DrawPortLayout->addSpacerItem(new QSpacerItem(10, 10, QSizePolicy::Expanding, QSizePolicy::Fixed));
    DrawPortLayout->addWidget(this->portBox__);
    DrawPortLayout->addSpacerItem(new QSpacerItem(16, 10, QSizePolicy::Fixed, QSizePolicy::Fixed));

    QWidget* UserName = new QWidget(this);
    UserName->setFixedHeight(60);
    SettingsLineInfoWidget* UserNameInfo = new SettingsLineInfoWidget(UserName, tr("Remote user name"));
    UserNameInfo->setTitleFontSize(14);
    this->Username__ = new ElaLineEdit(UserName);
    this->Username__->setAlignment(Qt::AlignCenter);
    this->Username__->setFixedSize(180, 30);
    this->Username__->setPlaceholderText("bitbot");
    QHBoxLayout* UserNameLayout = new QHBoxLayout(UserName);
    UserNameLayout->addWidget(UserNameInfo);
    UserNameLayout->addSpacerItem(new QSpacerItem(10, 10, QSizePolicy::Expanding, QSizePolicy::Fixed));
    UserNameLayout->addWidget(this->Username__);
    UserNameLayout->addSpacerItem(new QSpacerItem(16, 10, QSizePolicy::Fixed, QSizePolicy::Fixed));

    QWidget* UserPasswd = new QWidget(this);
    UserPasswd->setFixedHeight(60);
    SettingsLineInfoWidget* UserPasswdInfo = new SettingsLineInfoWidget(UserPasswd, tr("Remote password"));
    UserPasswdInfo->setTitleFontSize(14);
    this->Passwd__ = new ElaLineEdit(UserPasswd);
    this->Passwd__->setAlignment(Qt::AlignCenter);
    this->Passwd__->setFixedSize(180, 30);
    this->Passwd__->setPlaceholderText("bitbot");
    QHBoxLayout* UserPasswdLayout = new QHBoxLayout(UserPasswd);
    UserPasswdLayout->addWidget(UserPasswdInfo);
    UserPasswdLayout->addSpacerItem(new QSpacerItem(10, 10, QSizePolicy::Expanding, QSizePolicy::Fixed));
    UserPasswdLayout->addWidget(this->Passwd__);
    UserPasswdLayout->addSpacerItem(new QSpacerItem(16, 10, QSizePolicy::Fixed, QSizePolicy::Fixed));

    this->DrawAreaRemoteConn__ = new ElaDrawerArea(this);
    this->DrawAreaRemoteConn__->setDrawerHeader(DrawHead);
    this->DrawAreaRemoteConn__->addDrawer(DrawIP);
    this->DrawAreaRemoteConn__->addDrawer(DrawPort);
    this->DrawAreaRemoteConn__->addDrawer(UserName);
    this->DrawAreaRemoteConn__->addDrawer(UserPasswd);

    if (!this->isCurrentRemoteProtocal())
    {
        this->ipAddress__->setEnabled(false);
        this->portBox__->setEnabled(false);
        this->Username__->setEnabled(false);
        this->Passwd__->setEnabled(false);
        this->DrawAreaRemoteConn__->collpase();
    }
    else
    {
        this->ipAddress__->setEnabled(true);
        this->portBox__->setEnabled(true);
        this->Username__->setEnabled(true);
        this->Passwd__->setEnabled(true);
        this->DrawAreaRemoteConn__->expand();
    }

    QObject::connect(this, &SettingsBackEndWidget::connectionProtocalChanged, this, [this](const QString& text) {
        if (text == tr("ssh"))
        {
            this->ipAddress__->setEnabled(true);
            this->portBox__->setEnabled(true);
            this->Username__->setEnabled(true);
            this->Passwd__->setEnabled(true);
            this->DrawAreaRemoteConn__->expand();
        }
        else
        {
            this->ipAddress__->setEnabled(false);
            this->portBox__->setEnabled(false);
            this->Username__->setEnabled(false);
            this->Passwd__->setEnabled(false);
            this->DrawAreaRemoteConn__->collpase();
        }
        });
}

void SettingsBackEndWidget::InitDrawExecPath()
{
    QWidget* DrawHead = new QWidget(this);
    SettingsLineInfoWidget* DrawHeadInfo = new SettingsLineInfoWidget(DrawHead, tr("Backend executable"), tr("Setup backend executable path and name"), QChar(ElaIconType::Server));
    QHBoxLayout* DrawHeadLayout = new QHBoxLayout(DrawHead);
    DrawHeadLayout->addWidget(DrawHeadInfo);
    DrawHeadLayout->addSpacerItem(new QSpacerItem(10, 10, QSizePolicy::Expanding, QSizePolicy::Fixed));

    QWidget* DrawExecPath = new QWidget(this);
    DrawExecPath->setFixedHeight(60);
    SettingsLineInfoWidget* DrawExecPathInfo = new SettingsLineInfoWidget(DrawExecPath, tr("Executable path"));
    DrawExecPathInfo->setTitleFontSize(14);
    this->ExecPath__ = new ElaLineEdit(DrawExecPath);
    this->ExecPath__->setAlignment(Qt::AlignCenter);
    this->ExecPath__->setFixedSize(180, 30);
    this->ExecPath__->setPlaceholderText(QString("/home/bitbot"));
    QHBoxLayout* DrawExecPathLayout = new QHBoxLayout(DrawExecPath);
    DrawExecPathLayout->addWidget(DrawExecPathInfo);
    DrawExecPathLayout->addSpacerItem(new QSpacerItem(10, 10, QSizePolicy::Expanding, QSizePolicy::Fixed));
    DrawExecPathLayout->addWidget(this->ExecPath__);
    DrawExecPathLayout->addSpacerItem(new QSpacerItem(16, 10, QSizePolicy::Fixed, QSizePolicy::Fixed));


    QWidget* DrawExecName = new QWidget(this);
    DrawExecName->setFixedHeight(60);
    SettingsLineInfoWidget* DrawExecNameInfo = new SettingsLineInfoWidget(DrawExecName, tr("Executable name"));
    DrawExecNameInfo->setTitleFontSize(14);
    this->ExecName__ = new ElaLineEdit(DrawExecName);
    this->ExecName__->setAlignment(Qt::AlignCenter);
    this->ExecName__->setFixedSize(180, 30);
    this->ExecName__->setPlaceholderText(QString("main_app"));
    QHBoxLayout* DrawExecNameLayout = new QHBoxLayout(DrawExecName);
    DrawExecNameLayout->addWidget(DrawExecNameInfo);
    DrawExecNameLayout->addSpacerItem(new QSpacerItem(10, 10, QSizePolicy::Expanding, QSizePolicy::Fixed));
    DrawExecNameLayout->addWidget(this->ExecName__);
    DrawExecNameLayout->addSpacerItem(new QSpacerItem(16, 10, QSizePolicy::Fixed, QSizePolicy::Fixed));

    this->DrawAreaExecPath__ = new ElaDrawerArea(this);
    this->DrawAreaExecPath__->setDrawerHeader(DrawHead);
    this->DrawAreaExecPath__->addDrawer(DrawExecPath);
    this->DrawAreaExecPath__->addDrawer(DrawExecName);
    this->DrawAreaExecPath__->expand();
}

void SettingsBackEndWidget::InitProtocalSelection()
{
    this->ProtocalSelectionArea__ = new ElaScrollPageArea(this);
    SettingsLineInfoWidget* ProtocalInfo = new SettingsLineInfoWidget(this->ProtocalSelectionArea__, tr("Backend protocal"), tr("connect to remote robot via ssh or launch local robot"), QChar(ElaIconType::ArrowRightArrowLeft));
    this->Protocal__ = new ElaComboBox(this->ProtocalSelectionArea__);
    this->Protocal__->setFixedWidth(150);

    this->Protocal__->addItem(tr("local"));
    this->Protocal__->addItem(tr("ssh"));
    QObject::connect(this->Protocal__, &ElaComboBox::currentTextChanged, this, &SettingsBackEndWidget::connectionProtocalChanged);

    QHBoxLayout* layout = new QHBoxLayout(this->ProtocalSelectionArea__);
    layout->addWidget(ProtocalInfo);
    layout->addSpacerItem(new QSpacerItem(10, 10, QSizePolicy::Expanding, QSizePolicy::Fixed));
    layout->addWidget(this->Protocal__);
    layout->addSpacerItem(new QSpacerItem(16, 10, QSizePolicy::Fixed, QSizePolicy::Fixed));


}
