#include "PopupSettingsPage.h"
#include "qeventloop.h"
#include "QVBoxLayout"
#include "ElaScrollArea.h"
#include "ElaPushButton.h"
#include "ElaTheme.h"
#include "UI/widget/FluentMessageBox.hpp"
#include "Utils/Settings/SettingsHandler.h"
#include "QApplication"

PopupSettingsPage::PopupSettingsPage(QWidget* parent)
    :ElaWidget(parent)
{
    this->setAttribute(Qt::WA_DeleteOnClose);
    this->setWindowTitle(tr("Add new robot profile"));
    this->resize(768, 600);
    //this->setIsFixedSize(true);
    this->setWindowModality(Qt::WindowModality::ApplicationModal);

    ElaScrollArea* ScrollArea = new ElaScrollArea(this);
    ScrollArea->setIsGrabGesture(true, 0);
    ScrollArea->setWidgetResizable(true);
    QWidget* ScrollWidget = new QWidget(ScrollArea);
    ScrollWidget->setStyleSheet(R"(background-color:rgba(215, 101, 101, 0);)");
    ScrollArea->setWidget(ScrollWidget);
    QVBoxLayout* ScrollLayout = new QVBoxLayout(ScrollWidget);
    this->AvatarNameEditor__ = new AvatarNameEditor(ScrollWidget);
    this->FrontendSettings__ = new SettingsFrontEndWidget(ScrollWidget);
    this->BackendSettings__ = new SettingsBackEndWidget(ScrollWidget);
    this->DatabaseSettings__ = new SettingsDatabaseWidget(ScrollWidget);
    this->AutorunLaucher__ = new SettingsAutorunLauncher(ScrollWidget);
    ScrollLayout->addWidget(this->AvatarNameEditor__);
    ScrollLayout->addSpacerItem(new QSpacerItem(10, 10, QSizePolicy::Fixed, QSizePolicy::Fixed));
    ScrollLayout->addWidget(this->FrontendSettings__);
    ScrollLayout->addSpacerItem(new QSpacerItem(10, 10, QSizePolicy::Fixed, QSizePolicy::Fixed));
    ScrollLayout->addWidget(this->BackendSettings__);
    ScrollLayout->addSpacerItem(new QSpacerItem(10, 10, QSizePolicy::Fixed, QSizePolicy::Fixed));
    ScrollLayout->addWidget(this->DatabaseSettings__);
    ScrollLayout->addSpacerItem(new QSpacerItem(10, 10, QSizePolicy::Fixed, QSizePolicy::Fixed));
    ScrollLayout->addWidget(this->AutorunLaucher__);
    ScrollLayout->addSpacerItem(new QSpacerItem(10, 10, QSizePolicy::Fixed, QSizePolicy::Expanding));

    this->InitBottomButton();


    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->addSpacerItem(new QSpacerItem(10, 50, QSizePolicy::Expanding, QSizePolicy::Fixed));
    buttonLayout->addWidget(this->cancelButton__);
    buttonLayout->addWidget(this->confirmButton__);
    buttonLayout->addSpacerItem(new QSpacerItem(20, 50, QSizePolicy::Fixed, QSizePolicy::Fixed));

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(ScrollArea);
    mainLayout->addSpacerItem(new QSpacerItem(10, 4, QSizePolicy::Fixed, QSizePolicy::Fixed));
    mainLayout->addLayout(buttonLayout);
}

PopupSettingsPage::~PopupSettingsPage()
{

}

QString PopupSettingsPage::exec()
{
    QEventLoop loop;
    this->setWindowModality(Qt::WindowModality::ApplicationModal);
    this->show();
    QString newProfileName;
    QObject::connect(this, &PopupSettingsPage::NewProfileCreated, [&](QString ProfileName) {
        newProfileName = ProfileName;
        });
    QObject::connect(this, &PopupSettingsPage::destroyed, &loop, &QEventLoop::quit);
    loop.exec();
    return newProfileName;
}

void PopupSettingsPage::InitBottomButton()
{
    this->cancelButton__ = new ElaPushButton(this);
    this->cancelButton__->setText(tr("Cancel"));
    this->cancelButton__->setFixedSize(180, 35);
    QObject::connect(this->cancelButton__, &ElaPushButton::clicked, this, &PopupSettingsPage::close);

    this->confirmButton__ = new ElaPushButton(this);
    this->confirmButton__->setText(tr("Continue"));
    this->confirmButton__->setFixedSize(180, 35);
    QColor ButtonLightDefaultColor(eTheme->getThemeColor(ElaThemeType::ThemeMode::Light, ElaThemeType::ThemeColor::PrimaryNormal));
    QColor ButtonLightHoverColor(eTheme->getThemeColor(ElaThemeType::ThemeMode::Light, ElaThemeType::ThemeColor::PrimaryHover));
    QColor ButtonLightPressColor(eTheme->getThemeColor(ElaThemeType::ThemeMode::Light, ElaThemeType::ThemeColor::PrimaryPress));
    QColor ButtonLightTextColor(255, 255, 255);
    QColor ButtonDarkDefaultColor(eTheme->getThemeColor(ElaThemeType::ThemeMode::Dark, ElaThemeType::ThemeColor::PrimaryNormal));
    QColor ButtonDarkHoverColor(eTheme->getThemeColor(ElaThemeType::ThemeMode::Dark, ElaThemeType::ThemeColor::PrimaryHover));
    QColor ButtonDarkPressColor(eTheme->getThemeColor(ElaThemeType::ThemeMode::Dark, ElaThemeType::ThemeColor::PrimaryPress));
    QColor ButtonDarkTextColor(0, 0, 0);
    this->confirmButton__->setDarkDefaultColor(ButtonDarkDefaultColor);
    this->confirmButton__->setDarkHoverColor(ButtonDarkHoverColor);
    this->confirmButton__->setDarkPressColor(ButtonDarkPressColor);
    this->confirmButton__->setDarkTextColor(ButtonDarkTextColor);
    this->confirmButton__->setLightDefaultColor(ButtonLightDefaultColor);
    this->confirmButton__->setLightHoverColor(ButtonLightHoverColor);
    this->confirmButton__->setLightPressColor(ButtonLightPressColor);
    this->confirmButton__->setLightTextColor(ButtonLightTextColor);

    QObject::connect(this->confirmButton__, &ElaPushButton::clicked, this, &PopupSettingsPage::onConfirmClicked);


}

void PopupSettingsPage::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    auto _themeMode = eTheme->getThemeMode();
    painter.save();
    painter.setPen(ElaThemeColor(_themeMode, BasicBorder));
    painter.setBrush(ElaThemeColor(_themeMode, BasicBaseAlpha));
    QRect foregroundRect(0, height() - 70, width(), 70);
    painter.drawRect(foregroundRect);
    painter.restore();

    QWidget::paintEvent(event);

}

void PopupSettingsPage::onConfirmClicked()
{
    QString ProfileName = this->AvatarNameEditor__->getUserName();
    if (ProfileName.isEmpty())
    {
        FluentMessageBox::warningOk(this, tr("Empty Profile Name"), tr("Profile name cannot be empty."));
        return;
    }
    ProfileName += ".ini";
    QStringList ProfileList = ZSet->getUserList();
    if (ProfileList.contains(ProfileName))
    {
        FluentMessageBox::warningOk(this, tr("Duplicate Profile Name"), tr("Profile name already exists, please choose another name."));
        return;
    }

    RobotConfig_t config;
    config.Profile.Avatar = this->AvatarNameEditor__->getAvatarPath();
    config.Frontend.ip = this->FrontendSettings__->getIP();
    config.Frontend.port = this->FrontendSettings__->getPort();
    config.Backend.ip = this->BackendSettings__->getIP();
    config.Backend.Port = this->BackendSettings__->getPort();
    config.Backend.Username = this->BackendSettings__->getUsername();
    config.Backend.Passwd = this->BackendSettings__->getUserPasswd();
    config.Backend.isRemote = this->BackendSettings__->isCurrentRemoteProtocal();
    config.Backend.Path = this->BackendSettings__->getExecPath();
    config.Backend.Exec = this->BackendSettings__->getExecName();
    config.Backend.CacheRemoteData = this->DatabaseSettings__->getCacheData();
    config.Backend.DataViewerCachePath = this->DatabaseSettings__->getLocalCachePath();
    config.Backend.DataPath = this->DatabaseSettings__->getRemoteDataPath();
    config.AutoRunCommands = this->AutorunLaucher__->getCmdList();

    //TODO: add autorun config

    QString error_msg;
    if (!ZSet->SaveAllConfig(config, ProfileName, error_msg))
    {
        FluentMessageBox::warningOk(this, tr("Save Profile Failed"), tr("Failed to save profile: %1").arg(error_msg));
        return;
    }

    emit this->NewProfileCreated(ProfileName);
    qApp->processEvents();
    this->close();
}
