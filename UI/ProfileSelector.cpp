#include "ProfileSelector.h"
#include "QEventLoop"
#include "QApplication"
#include "ElaInteractiveCard.h"
#include "QVBoxLayout"
#include "ElaText.h"
#include "QHBoxLayout"
#include "ElaTheme.h"
#include "ElaScrollArea.h"
#include "Utils/Settings/SettingsHandler.h"
#include "QPainter"
#include "ElaTheme.h"

QString ProfileSelector::Select()
{
    ProfileSelector* selector = new ProfileSelector();
    selector->setAttribute(Qt::WA_DeleteOnClose);
    selector->setWindowModality(Qt::WindowModality::ApplicationModal);
    selector->setFixedSize(400, 590);
    selector->setIsFixedSize(true);
    selector->show();
    QString selectedProfile;

    QEventLoop loop;
    QObject::connect(selector, &ProfileSelector::selected, [&loop, &selectedProfile](QString profileName) {
        selectedProfile = profileName;
        });
    QObject::connect(selector, &ProfileSelector::destroyed, &loop, &QEventLoop::quit);
    loop.exec();
    qDebug() << "Selected Profile:" << selectedProfile;
    return selectedProfile;
}

ProfileSelector::ProfileSelector(QWidget* parent)
    :ElaWidget(parent)
{
    this->setWindowTitle(tr("robot profiles"));
    //创建标题
    ElaText* title = new ElaText(this);
    title->setText(QString("  ") + tr("Select your robot profile"));
    title->setTextStyle(ElaTextType::Subtitle);

    //中央滚动区域
    ElaScrollArea* CentralScrollArea = new ElaScrollArea(this);
    CentralScrollArea->setIsGrabGesture(true, 0);
    CentralScrollArea->setWidgetResizable(true);
    auto scrollAreaWidgetContents = new QWidget(CentralScrollArea);
    CentralScrollArea->setWidget(scrollAreaWidgetContents);
    scrollAreaWidgetContents->setStyleSheet(R"(background-color:rgba(0,0,0,0);)");

    // 初始化当前配置和标题
    ElaText* current_sub_title = new ElaText(scrollAreaWidgetContents);
    current_sub_title->setText(QString("   ") + tr("Use this profile"));
    current_sub_title->setTextStyle(ElaTextType::BodyStrong);
    current_sub_title->setTextPixelSize(16);

    this->InitCurrentProfile(scrollAreaWidgetContents);

    //初始化其余配置和标题
    ElaText* others_sub_title = new ElaText(scrollAreaWidgetContents);
    others_sub_title->setText(QString("   ") + tr("Use a different profile"));
    others_sub_title->setTextStyle(ElaTextType::BodyStrong);
    others_sub_title->setTextPixelSize(16);

    this->InitAvailableProfiles(scrollAreaWidgetContents);

    //创建滚动区域布局
    QVBoxLayout* CentralScrollLay = new QVBoxLayout(scrollAreaWidgetContents);
    CentralScrollLay->addWidget(current_sub_title);
    CentralScrollLay->addWidget(this->CurrentProfileCard);
    CentralScrollLay->addSpacerItem(new QSpacerItem(20, 16, QSizePolicy::Fixed, QSizePolicy::Fixed));
    CentralScrollLay->addWidget(others_sub_title);
    CentralScrollLay->addSpacerItem(new QSpacerItem(20, 16, QSizePolicy::Fixed, QSizePolicy::Fixed));
    for (auto item : this->AvailableProfileCards)
    {
        CentralScrollLay->addWidget(item);
    }

    CentralScrollLay->addSpacerItem(new QSpacerItem(20, 20, QSizePolicy::Fixed, QSizePolicy::Expanding));

    //初始化底部区域
    this->InitSelectButton();

    QHBoxLayout* SelectedButtonLayout = new QHBoxLayout();
    SelectedButtonLayout->addSpacerItem(new QSpacerItem(20, 20, QSizePolicy::Expanding, QSizePolicy::Fixed));
    SelectedButtonLayout->addWidget(this->SelectedButton);
    SelectedButtonLayout->addSpacerItem(new QSpacerItem(20, 20, QSizePolicy::Fixed, QSizePolicy::Fixed));

    //添加整体布局
    QVBoxLayout* lay = new QVBoxLayout(this);
    lay->addWidget(title);
    lay->addSpacerItem(new QSpacerItem(20, 16, QSizePolicy::Fixed, QSizePolicy::Fixed));
    lay->addWidget(CentralScrollArea);

    lay->addSpacerItem(new QSpacerItem(20, 16, QSizePolicy::Preferred, QSizePolicy::Fixed));
    lay->addLayout(SelectedButtonLayout);
    lay->addSpacerItem(new QSpacerItem(20, 10, QSizePolicy::Preferred, QSizePolicy::Fixed));


}

ProfileSelector::~ProfileSelector()
{

}

void ProfileSelector::InitSelectButton()
{
    this->SelectedButton = new ElaPushButton(tr("Continue"), this);
    this->SelectedButton->setFixedWidth(180);
    QColor ButtonLightDefaultColor(eTheme->getThemeColor(ElaThemeType::ThemeMode::Light, ElaThemeType::ThemeColor::PrimaryNormal));
    QColor ButtonLightHoverColor(eTheme->getThemeColor(ElaThemeType::ThemeMode::Light, ElaThemeType::ThemeColor::PrimaryHover));
    QColor ButtonLightPressColor(eTheme->getThemeColor(ElaThemeType::ThemeMode::Light, ElaThemeType::ThemeColor::PrimaryPress));
    QColor ButtonLightTextColor(255, 255, 255);
    QColor ButtonDarkDefaultColor(eTheme->getThemeColor(ElaThemeType::ThemeMode::Dark, ElaThemeType::ThemeColor::PrimaryNormal));
    QColor ButtonDarkHoverColor(eTheme->getThemeColor(ElaThemeType::ThemeMode::Dark, ElaThemeType::ThemeColor::PrimaryHover));
    QColor ButtonDarkPressColor(eTheme->getThemeColor(ElaThemeType::ThemeMode::Dark, ElaThemeType::ThemeColor::PrimaryPress));
    QColor ButtonDarkTextColor(0, 0, 0);
    this->SelectedButton->setDarkDefaultColor(ButtonDarkDefaultColor);
    this->SelectedButton->setDarkHoverColor(ButtonDarkHoverColor);
    this->SelectedButton->setDarkPressColor(ButtonDarkPressColor);
    this->SelectedButton->setDarkTextColor(ButtonDarkTextColor);
    this->SelectedButton->setLightDefaultColor(ButtonLightDefaultColor);
    this->SelectedButton->setLightHoverColor(ButtonLightHoverColor);
    this->SelectedButton->setLightPressColor(ButtonLightPressColor);
    this->SelectedButton->setLightTextColor(ButtonLightTextColor);

    QObject::connect(this->SelectedButton, &ElaPushButton::clicked, this, [this]() {
        emit selected(this->CurrentSelectedCard->getProfileName());
        this->close();
        });
}

void ProfileSelector::InitCurrentProfile(QWidget* parent)
{
    auto [name, ip, avatar] = ZSet->getUserProfileInfo();

    this->CurrentProfileCard = new ProfileIDCard(parent, name, ip, avatar);
    this->CurrentProfileCard->setSelected(true);
    this->CurrentSelectedCard = this->CurrentProfileCard;
    QObject::connect(this->CurrentProfileCard, &ProfileIDCard::clicked, this, [this]() {
        qDebug() << this->CurrentProfileCard->getProfileName();
        this->SwitchSelectedCard(this->CurrentProfileCard);
        });
}

void ProfileSelector::InitAvailableProfiles(QWidget* parent)
{
    auto profiles = ZSet->getUserProfileInfos(true);
    for (int i = 0;i < profiles.size();i++)
    {
        auto [name, ip, avatar] = profiles[i];
        ProfileIDCard* card = new ProfileIDCard(parent, name, ip, avatar);
        QObject::connect(card, &ProfileIDCard::clicked, this, [this, card]() {
            qDebug() << card->getProfileName();
            this->SwitchSelectedCard(card);
            });
        this->AvailableProfileCards.append(card);
    }

}

void ProfileSelector::SwitchSelectedCard(ProfileIDCard* card)
{
    this->CurrentProfileCard->setSelected(false);
    this->CurrentProfileCard->update();
    for (auto card : this->AvailableProfileCards)
    {
        card->setSelected(false);
        card->update();
    }

    card->setSelected(true);
    this->CurrentSelectedCard = card;
}



ProfileIDCard::ProfileIDCard(QWidget* parent, QString profileName, QString ipAddress, QString avatar)
    :ElaInteractiveCard(parent),
    m_selected(false)
{
    this->m_profileName = profileName;
    qDebug() << this->m_profileName;
    this->setTitle(profileName.split(".ini").first());
    this->setSubTitle(QString("ip: ") + ipAddress);
    this->setCardPixmap(QPixmap(avatar));
}

ProfileIDCard::~ProfileIDCard()
{

}

void ProfileIDCard::setSelected(bool select)
{
    this->m_selected = select;
}

bool ProfileIDCard::isSelected()
{
    return this->m_selected;
}

QString ProfileIDCard::getProfileName()
{
    return this->m_profileName;
}

void ProfileIDCard::paintEvent(QPaintEvent* event)
{
    ElaInteractiveCard::paintEvent(event);
    QPainter painter(this);
    painter.save();
    painter.setRenderHints(QPainter::SmoothPixmapTransform | QPainter::Antialiasing | QPainter::TextAntialiasing);
    painter.setPen(Qt::NoPen);
    painter.setBrush(this->isSelected() ? ElaThemeColor(eTheme->getThemeMode(), BasicHoverAlpha) : Qt::transparent);
    painter.drawRoundedRect(rect(), this->getBorderRadius(), this->getBorderRadius());
    painter.restore();
}
