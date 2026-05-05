#include "AvatarNameEditor.h"
#include "QVBoxLayout"
#include "QHBoxLayout"
#include "ElaTheme.h"
#include "Utils/Settings/SettingsHandler.h"

AvatarNameEditor::AvatarNameEditor(QWidget* parent)
    :QWidget(parent)
{

    this->title__ = new ElaText(this);
    this->title__->setTextStyle(ElaTextType::BodyStrong);
    this->title__->setText(tr("Robot Profile"));
    this->title__->setTextPixelSize(18);
    this->title__->setAlignment(Qt::AlignTop);

    this->LargeAvatarLabel__ = new CustomImageLabel(this);
    this->LargeAvatarLabel__->setFixedSize(100, 100);

    this->userNameInfo__ = new ElaText(this);
    userNameInfo__->setText(tr("Robot Name"));
    userNameInfo__->setTextStyle(ElaTextType::BodyStrong);
    userNameInfo__->setTextPixelSize(16);
    userNameInfo__->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    this->userName__ = new ElaLineEdit(this);
    this->userName__->setFixedHeight(35);
    this->userName__->setFixedWidth(250);
    QFont userNameFont = this->userName__->font();
    userNameFont.setPixelSize(16);
    this->userName__->setFont(userNameFont);
    this->userName__->setPlaceholderText(tr("Enter robot name"));
    this->userName__->setAlignment(Qt::AlignCenter);

    this->avatarSelector__ = new AvatarSelector(this);
    connect(avatarSelector__, &AvatarSelector::avatarChanged, this, &AvatarNameEditor::onAvatarChanged);

    this->avatarSelector__->SetCurrentAvatar(SettingsHandler::getRandomAvatar());
    QPixmap avatar_image = this->avatarSelector__->GetCurrentAvatar().isEmpty() ? QPixmap("") : QPixmap(this->avatarSelector__->GetCurrentAvatar());
    this->LargeAvatarLabel__->setPixmap(avatar_image, 100, 100);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(this->title__, 0, Qt::AlignTop | Qt::AlignLeft);
    mainLayout->addSpacerItem(new QSpacerItem(10, 10, QSizePolicy::Fixed, QSizePolicy::Fixed));
    mainLayout->addWidget(this->LargeAvatarLabel__, 0, Qt::AlignHCenter);
    mainLayout->addSpacerItem(new QSpacerItem(10, 10, QSizePolicy::Fixed, QSizePolicy::Fixed));
    mainLayout->addWidget(userNameInfo__, 0, Qt::AlignHCenter);
    mainLayout->addWidget(this->userName__, 0, Qt::AlignHCenter);
    mainLayout->addSpacerItem(new QSpacerItem(10, 10, QSizePolicy::Fixed, QSizePolicy::Fixed));
    mainLayout->addWidget(this->avatarSelector__);
    //mainLayout->addSpacerItem(new QSpacerItem(10, 10, QSizePolicy::Fixed, QSizePolicy::Expanding));
}

AvatarNameEditor::~AvatarNameEditor()
{
}

QString AvatarNameEditor::getAvatarPath() const
{
    return this->avatarSelector__->GetCurrentAvatar();
}

void AvatarNameEditor::setAvatarPath(QString path)
{
    this->avatarSelector__->SetCurrentAvatar(path);
}

QString AvatarNameEditor::getUserName() const
{
    return this->userName__->text();
}

void AvatarNameEditor::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    auto _themeMode = eTheme->getThemeMode();
    painter.save();
    painter.setPen(ElaThemeColor(_themeMode, BasicBorder));
    painter.setBrush(ElaThemeColor(_themeMode, BasicBaseAlpha));
    QRect foregroundRect(1, this->title__->y() + this->title__->height() + 8, width() - 2, height() - 2 - (this->title__->y() + this->title__->height() + 8));
    painter.drawRoundedRect(foregroundRect, 6, 6);
    painter.restore();

    QWidget::paintEvent(event);
}

void AvatarNameEditor::onAvatarChanged(QString path)
{
    QPixmap avatar_image = path.isEmpty() ? QPixmap("") : QPixmap(path);
    this->LargeAvatarLabel__->setPixmap(avatar_image, 100, 100);
}

void AvatarNameEditor::setUserName(QString name)
{
    this->userName__->setText(name);
}

void AvatarNameEditor::setNameEditable(bool editable)
{
    this->userName__->setEnabled(editable);
}

