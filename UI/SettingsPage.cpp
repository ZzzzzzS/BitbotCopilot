#include "SettingsPage.h"
#include "QVBoxLayout"
#include "QStackedWidget"
#include "QHBoxLayout"
#include "ElaPushButton.h"
#include "ElaText.h"
#include "ElaBreadcrumbBar.h"
#include "ElaTheme.h"
#include "UI/SettingPageWidget/RobotConfigForm.h"
#include "UI/widget/ProfileIDCard.h"
#include "UI/widget/FluentMessageBox.hpp"
#include "Utils/Settings/SettingsHandler.h"
#include "QApplication"
#include "QProcess"
#include "QFile"

SettingsPage::SettingsPage(QWidget* parent)
    : ElaScrollPage(parent),
    currentProfileCard__(nullptr),
    addNewProfileCard__(nullptr),
    isNewProfileMode__(false)
{
    this->setTitleVisible(false);
    
    QWidget* centralWidget = new QWidget(this);
    QVBoxLayout* centerVLayout = new QVBoxLayout(centralWidget);
    centerVLayout->setContentsMargins(0, 0, 0, 0);
    
    // Custom breadcrumb bar
    this->breadcrumbBar__ = new ElaBreadcrumbBar(this);
    this->breadcrumbBar__->setBreadcrumbList({tr("Robot Profiles")});
    QObject::connect(this->breadcrumbBar__, &ElaBreadcrumbBar::breadcrumbClicked, this, [this](QString breadcrumb, QStringList lastBreadcrumbList) {
        if (breadcrumb == tr("Robot Profiles") && this->stackedWidget__->currentIndex() != 0)
        {
            this->backToSelectionPage();
        }
    });
    centerVLayout->addWidget(this->breadcrumbBar__);
    centerVLayout->addSpacerItem(new QSpacerItem(10, 10, QSizePolicy::Fixed, QSizePolicy::Fixed));
    
    this->stackedWidget__ = new QStackedWidget(this);
    
    this->initSelectionPage();
    this->initEditorPage();
    
    centerVLayout->addWidget(this->stackedWidget__);
    this->addCentralWidget(centralWidget, true, false, 0);
}

SettingsPage::~SettingsPage()
{
}

void SettingsPage::backToSelectionPage()
{
    this->stackedWidget__->setCurrentIndex(0);
    this->breadcrumbBar__->setBreadcrumbList({tr("Robot Profiles")});
    this->reloadUserCards();
}

void SettingsPage::initSelectionPage()
{
    this->selectionPage__ = new QWidget(this);
    QVBoxLayout* mainLayout = new QVBoxLayout(this->selectionPage__);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    
    this->cardsLayout__ = new QVBoxLayout();
    this->cardsLayout__->setContentsMargins(0, 0, 0, 0);
    
    mainLayout->addLayout(this->cardsLayout__);
    mainLayout->addSpacerItem(new QSpacerItem(10, 10, QSizePolicy::Fixed, QSizePolicy::Expanding));
    
    this->stackedWidget__->addWidget(this->selectionPage__);
    
    this->reloadUserCards();
}

void SettingsPage::reloadUserCards()
{
    // Clear old widgets from layout
    QLayoutItem* item;
    while ((item = this->cardsLayout__->takeAt(0)) != nullptr)
    {
        if (item->widget())
        {
            item->widget()->setParent(nullptr);
            delete item->widget();
        }
        else
        {
            delete item;
        }
    }
    
    this->otherProfileCards__.clear();
    this->currentProfileCard__ = nullptr;
    this->addNewProfileCard__ = nullptr;
    
    // Load current profile
    auto [currentName, currentIp, currentAvatar] = ZSet->getUserProfileInfo();
    QString currentProfileFullName = ZSet->getCurrentProfileName();
    
    // Current Profile section
    ElaText* currentTitle = new ElaText(this->selectionPage__);
    currentTitle->setText(tr("Current Profile"));
    currentTitle->setTextStyle(ElaTextType::BodyStrong);
    currentTitle->setTextPixelSize(16);
    this->cardsLayout__->addWidget(currentTitle);
    
    this->currentProfileCard__ = new ProfileIDCard(this->selectionPage__, currentProfileFullName, QString("ip: ") + currentIp, currentAvatar);
    QObject::connect(this->currentProfileCard__, &ProfileIDCard::clicked, this, &SettingsPage::onCardClicked);
    this->cardsLayout__->addWidget(this->currentProfileCard__);
    this->cardsLayout__->addSpacerItem(new QSpacerItem(10, 16, QSizePolicy::Fixed, QSizePolicy::Fixed));
    
    // Other Profiles section
    ElaText* othersTitle = new ElaText(this->selectionPage__);
    othersTitle->setText(tr("Other Profiles"));
    othersTitle->setTextStyle(ElaTextType::BodyStrong);
    othersTitle->setTextPixelSize(16);
    this->cardsLayout__->addWidget(othersTitle);
    
    auto profiles = ZSet->getUserProfileInfos(true);
    for (const auto& profile : profiles)
    {
        auto [name, ip, avatar] = profile;
        ProfileIDCard* card = new ProfileIDCard(this->selectionPage__, name, QString("ip: ") + ip, avatar);
        QObject::connect(card, &ProfileIDCard::clicked, this, &SettingsPage::onCardClicked);
        this->otherProfileCards__.append(card);
        this->cardsLayout__->addWidget(card);
    }
    
    // Add New card
    this->addNewProfileCard__ = new ProfileIDCard(this->selectionPage__, tr("Add new robot profile"), tr("Add remote robot or local robot"), ":UI/Image/addNewRobotProfile.png");
    QObject::connect(this->addNewProfileCard__, &ProfileIDCard::clicked, this, &SettingsPage::onAddNewClicked);
    this->cardsLayout__->addWidget(this->addNewProfileCard__);
}

void SettingsPage::onCardClicked()
{
    ProfileIDCard* clickedCard = qobject_cast<ProfileIDCard*>(sender());
    if (!clickedCard)
        return;
    
    QString profileName = clickedCard->getProfileName();
    this->loadProfileToEditor(profileName);
}

void SettingsPage::onAddNewClicked()
{
    // Enter new profile mode
    this->isNewProfileMode__ = true;
    this->editingProfileName__.clear();
    
    // Set defaults
    this->configForm__->setProfileName("");
    this->configForm__->setProfileNameEditable(true);
    this->configForm__->setAvatarPath(SettingsHandler::getRandomAvatar());
    this->configForm__->setAutorunVisible(false);
    
    // Reset other fields to defaults
    RobotConfig_t defaultConfig;
    defaultConfig.Profile.Avatar = SettingsHandler::getRandomAvatar();
    defaultConfig.Frontend.ip = "127.0.0.1";
    defaultConfig.Frontend.port = 12888;
    defaultConfig.Backend.ip = "";
    defaultConfig.Backend.Port = 0;
    defaultConfig.Backend.Username = "";
    defaultConfig.Backend.Passwd = "";
    defaultConfig.Backend.isRemote = false;
    defaultConfig.Backend.Path = "/home/bitbot";
    defaultConfig.Backend.Exec = "main_app";
    defaultConfig.Backend.CacheRemoteData = false;
    defaultConfig.Backend.DataViewerCachePath = "";
    defaultConfig.Backend.DataPath = "/home";
    
    this->configForm__->loadFromConfig(defaultConfig);
    
    this->breadcrumbBar__->setBreadcrumbList({tr("Robot Profiles"), tr("Add New Robot Profile")});
    this->setEditorDeleteVisible(false);
    this->stackedWidget__->setCurrentIndex(1);
}

void SettingsPage::loadProfileToEditor(const QString& profileName)
{
    this->isNewProfileMode__ = false;
    this->editingProfileName__ = profileName;
    
    RobotConfig_t config;
    QString errorMsg;
    if (!ZSet->GetAllConfig(config, profileName, errorMsg))
    {
        FluentMessageBox::warningOk(this, tr("Load Failed"), tr("Failed to load profile: %1").arg(errorMsg));
        return;
    }
    
    this->configForm__->loadFromConfig(config);
    this->configForm__->setProfileName(QString(profileName).remove(".ini"));
    this->configForm__->setProfileNameEditable(false);
    this->configForm__->setAutorunVisible(false);
    
    QString displayName = QString(profileName).remove(".ini");
    this->breadcrumbBar__->setBreadcrumbList({tr("Robot Profiles"), tr("Edit Profile: %1").arg(displayName)});
    
    // Cannot delete current user
    QString currentProfile = ZSet->getCurrentProfileName();
    if (profileName == currentProfile)
    {
        this->setEditorDeleteVisible(false);
    }
    else
    {
        this->setEditorDeleteVisible(true);
    }
    
    this->stackedWidget__->setCurrentIndex(1);
}

void SettingsPage::initEditorPage()
{
    this->editorPage__ = new QWidget(this);
    QVBoxLayout* layout = new QVBoxLayout(this->editorPage__);
    layout->setContentsMargins(0, 0, 0, 0);
    
    // Config form
    this->configForm__ = new RobotConfigForm(this->editorPage__);
    
    // Bottom bar: Delete + Save buttons
    QHBoxLayout* bottomBar = new QHBoxLayout();
    bottomBar->addSpacerItem(new QSpacerItem(10, 10, QSizePolicy::Expanding, QSizePolicy::Fixed));
    
    this->editorDeleteButton__ = new ElaPushButton(this->editorPage__);
    this->editorDeleteButton__->setText(tr("Delete Profile"));
    this->editorDeleteButton__->setFixedSize(180, 35);
    
    // Set delete button to red/warning color
    QColor DeleteLightDefaultColor(220, 53, 69);
    QColor DeleteLightHoverColor(200, 35, 51);
    QColor DeleteLightPressColor(180, 25, 41);
    QColor DeleteLightTextColor(255, 255, 255);
    QColor DeleteDarkDefaultColor(220, 53, 69);
    QColor DeleteDarkHoverColor(200, 35, 51);
    QColor DeleteDarkPressColor(180, 25, 41);
    QColor DeleteDarkTextColor(255, 255, 255);
    this->editorDeleteButton__->setDarkDefaultColor(DeleteDarkDefaultColor);
    this->editorDeleteButton__->setDarkHoverColor(DeleteDarkHoverColor);
    this->editorDeleteButton__->setDarkPressColor(DeleteDarkPressColor);
    this->editorDeleteButton__->setDarkTextColor(DeleteDarkTextColor);
    this->editorDeleteButton__->setLightDefaultColor(DeleteLightDefaultColor);
    this->editorDeleteButton__->setLightHoverColor(DeleteLightHoverColor);
    this->editorDeleteButton__->setLightPressColor(DeleteLightPressColor);
    this->editorDeleteButton__->setLightTextColor(DeleteLightTextColor);
    
    this->editorSaveButton__ = new ElaPushButton(this->editorPage__);
    this->editorSaveButton__->setText(tr("Save"));
    this->editorSaveButton__->setFixedSize(180, 35);
    
    QColor SaveButtonLightDefaultColor(eTheme->getThemeColor(ElaThemeType::ThemeMode::Light, ElaThemeType::ThemeColor::PrimaryNormal));
    QColor SaveButtonLightHoverColor(eTheme->getThemeColor(ElaThemeType::ThemeMode::Light, ElaThemeType::ThemeColor::PrimaryHover));
    QColor SaveButtonLightPressColor(eTheme->getThemeColor(ElaThemeType::ThemeMode::Light, ElaThemeType::ThemeColor::PrimaryPress));
    QColor SaveButtonLightTextColor(255, 255, 255);
    QColor SaveButtonDarkDefaultColor(eTheme->getThemeColor(ElaThemeType::ThemeMode::Dark, ElaThemeType::ThemeColor::PrimaryNormal));
    QColor SaveButtonDarkHoverColor(eTheme->getThemeColor(ElaThemeType::ThemeMode::Dark, ElaThemeType::ThemeColor::PrimaryHover));
    QColor SaveButtonDarkPressColor(eTheme->getThemeColor(ElaThemeType::ThemeMode::Dark, ElaThemeType::ThemeColor::PrimaryPress));
    QColor SaveButtonDarkTextColor(0, 0, 0);
    this->editorSaveButton__->setDarkDefaultColor(SaveButtonDarkDefaultColor);
    this->editorSaveButton__->setDarkHoverColor(SaveButtonDarkHoverColor);
    this->editorSaveButton__->setDarkPressColor(SaveButtonDarkPressColor);
    this->editorSaveButton__->setDarkTextColor(SaveButtonDarkTextColor);
    this->editorSaveButton__->setLightDefaultColor(SaveButtonLightDefaultColor);
    this->editorSaveButton__->setLightHoverColor(SaveButtonLightHoverColor);
    this->editorSaveButton__->setLightPressColor(SaveButtonLightPressColor);
    this->editorSaveButton__->setLightTextColor(SaveButtonLightTextColor);
    
    bottomBar->addWidget(this->editorDeleteButton__);
    bottomBar->addSpacerItem(new QSpacerItem(20, 10, QSizePolicy::Fixed, QSizePolicy::Fixed));
    bottomBar->addWidget(this->editorSaveButton__);
    bottomBar->addSpacerItem(new QSpacerItem(10, 10, QSizePolicy::Expanding, QSizePolicy::Fixed));
    
    layout->addWidget(this->configForm__);
    layout->addSpacerItem(new QSpacerItem(10, 20, QSizePolicy::Fixed, QSizePolicy::Fixed));
    layout->addLayout(bottomBar);
    layout->addSpacerItem(new QSpacerItem(10, 10, QSizePolicy::Fixed, QSizePolicy::Expanding));
    
    QObject::connect(this->editorSaveButton__, &ElaPushButton::clicked, this, &SettingsPage::onEditorSaveClicked);
    QObject::connect(this->editorDeleteButton__, &ElaPushButton::clicked, this, &SettingsPage::onEditorDeleteClicked);
    
    this->stackedWidget__->addWidget(this->editorPage__);
}

void SettingsPage::setEditorDeleteVisible(bool visible)
{
    this->editorDeleteButton__->setVisible(visible);
}

void SettingsPage::onEditorSaveClicked()
{
    if (this->isNewProfileMode__)
    {
        // New profile validation
        QString profileName = this->configForm__->profileName();
        if (profileName.isEmpty())
        {
            FluentMessageBox::warningOk(this, tr("Empty Profile Name"), tr("Profile name cannot be empty."));
            return;
        }
        profileName += ".ini";
        
        QStringList existingProfiles = ZSet->getUserList();
        if (existingProfiles.contains(profileName))
        {
            FluentMessageBox::warningOk(this, tr("Duplicate Profile Name"), tr("Profile name already exists, please choose another name."));
            return;
        }
        
        RobotConfig_t config;
        this->configForm__->saveToConfig(config);
        
        QString errorMsg;
        if (!ZSet->SaveAllConfig(config, profileName, errorMsg))
        {
            FluentMessageBox::warningOk(this, tr("Save Failed"), tr("Failed to save profile: %1").arg(errorMsg));
            return;
        }
        
        // Add to user list
        existingProfiles.append(profileName);
        ZSet->updateUserList(existingProfiles);
        
        FluentMessageBox::informationOk(this, tr("Save Successful"), tr("New profile \"%1\" has been created.").arg(QString(profileName).remove(".ini")));
        this->backToSelectionPage();
    }
    else
    {
        // Edit existing profile
        QString profileName = this->editingProfileName__;
        if (profileName.isEmpty())
        {
            return;
        }
        
        RobotConfig_t config;
        this->configForm__->saveToConfig(config);
        
        QString errorMsg;
        if (!ZSet->SaveAllConfig(config, profileName, errorMsg))
        {
            FluentMessageBox::warningOk(this, tr("Save Failed"), tr("Failed to save profile: %1").arg(errorMsg));
            return;
        }
        
        QString currentProfile = ZSet->getCurrentProfileName();
        if (profileName == currentProfile)
        {
            // Editing current profile: restart application
            FluentMessageBox::informationOk(this, tr("Save Successful"), 
                tr("Configuration saved. Application will restart to apply changes."));
            
            QProcess::startDetached(QCoreApplication::applicationFilePath(), 
                QStringList() << "--no_splash", 
                QCoreApplication::applicationDirPath());
            qApp->quit();
        }
        else
        {
            FluentMessageBox::informationOk(this, tr("Save Successful"), 
                tr("Configuration saved successfully."));
            this->backToSelectionPage();
        }
    }
}

void SettingsPage::onEditorDeleteClicked()
{
    if (this->isNewProfileMode__)
        return;
    
    QString profileName = this->editingProfileName__;
    if (profileName.isEmpty())
        return;
    
    QString currentProfile = ZSet->getCurrentProfileName();
    if (profileName == currentProfile)
    {
        FluentMessageBox::warningOk(this, tr("Cannot Delete"), tr("Cannot delete the currently active profile."));
        return;
    }
    
    if (!FluentMessageBox::questionOkCancel(this, tr("Delete Profile"), 
        tr("Are you sure you want to delete profile \"%1\"? This action cannot be undone.").arg(QString(profileName).remove(".ini"))))
    {
        return;
    }
    
    // Delete profile file
    QString profileFilePath = QCoreApplication::applicationDirPath() + "/profile/" + profileName;
    QFile profileFile(profileFilePath);
    if (profileFile.exists())
    {
        profileFile.remove();
    }
    
    // Update user list
    QStringList userList = ZSet->getUserList();
    userList.removeAll(profileName);
    ZSet->updateUserList(userList);
    
    FluentMessageBox::informationOk(this, tr("Deleted"), tr("Profile \"%1\" has been deleted.").arg(QString(profileName).remove(".ini")));
    
    this->backToSelectionPage();
}
