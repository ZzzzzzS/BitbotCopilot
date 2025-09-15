#include "AvatarSelector.h"
#include "QVBoxLayout"
#include "QHBoxLayout"
#include "QEvent"
#include "QMouseEvent"
#include "QPainter"
#include "ElaTheme.h"
#include "Utils/Settings/SettingsHandler.h"

AvatarSelector::AvatarSelector(QWidget* parent)
    :QWidget(parent)
{
    flowLayout__ = new ElaFlowLayout(this);
    setLayout(flowLayout__);
    LoadAvatars();

}

AvatarSelector::~AvatarSelector()
{

}

bool AvatarSelector::SetCurrentAvatar(QString avatarPath)
{
    for (AvatarItem* item : avatarItems__)
    {
        if (item->GetAvatarPath() == avatarPath)
        {
            SwitchSelectedItem(item);
            return true;
        }
    }
    return false;
}

QString AvatarSelector::GetCurrentAvatar()
{
    return currentSelectedItem__ ? currentSelectedItem__->GetAvatarPath() : QString();
}

void AvatarSelector::LoadAvatars()
{
    QStringList avatarPaths = SettingsHandler::getAvailableAvatars();

    for (const QString& path : avatarPaths)
    {
        AvatarItem* item = new AvatarItem(this, path);
        connect(item, &AvatarItem::clicked, this, &AvatarSelector::SwitchSelectedItem);
        flowLayout__->addWidget(item);
        avatarItems__.append(item);
    }

    if (!avatarItems__.isEmpty())
    {
        currentSelectedItem__ = avatarItems__.first();
        currentSelectedItem__->SetSelected(true);
    }
}

void AvatarSelector::SwitchSelectedItem(AvatarItem* item)
{
    if (currentSelectedItem__ != item)
    {
        if (currentSelectedItem__)
        {
            currentSelectedItem__->SetSelected(false);
        }
        currentSelectedItem__ = item;
        currentSelectedItem__->SetSelected(true);

        emit avatarChanged(currentSelectedItem__->isSelected() ? currentSelectedItem__->GetAvatarPath() : QString());
    }
}

AvatarItem::AvatarItem(QWidget* parent, QString avatarPath)
    :QWidget(parent), avatarPath__(avatarPath)
{
    setMouseTracking(true);
    this->setStyleSheet("background-color:transparent;");

    this->avatarPixmap__ = QPixmap(avatarPath__);
    this->setFixedSize(64, 64);
}

AvatarItem::~AvatarItem()
{

}


void AvatarItem::paintEvent(QPaintEvent* event)
{
    bool isUnderMouse = underMouse();
    auto _themeMode = eTheme->getThemeMode();
    QPainter painter(this);
    painter.save();
    painter.setRenderHint(QPainter::Antialiasing, false);   // 1 px 时关闭 AA 更锐利



    if (isUnderMouse)
    {
        // draw hover effect, with a light border
        painter.setPen(ElaThemeColor(_themeMode, BasicBorder));
        painter.setBrush(ElaThemeColor(_themeMode, BasicPressAlpha));
    }
    else
    {
        painter.setPen(ElaThemeColor(_themeMode, BasicBorder));
        painter.setBrush(ElaThemeColor(_themeMode, BasicBaseAlpha));
    }

    if (this->isSelected__)
    {
        QColor borderColor = ElaThemeColor(_themeMode, PrimaryNormal);
        QPen borderPen(borderColor, 3, Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin);
        borderPen.setCosmetic(true);
        painter.setPen(borderPen);
        painter.setBrush(ElaThemeColor(_themeMode, BasicSelectedAlpha));
    }
    QRect foregroundRect(2, 2, width() - 4, height() - 4);
    //QRect foregroundRect(0, 0, width(), height());
    painter.drawRoundedRect(foregroundRect, 2, 2);
    //painter.drawRect(foregroundRect);

    if (!this->avatarPixmap__.isNull())
    {
        QRect avatarRect(2, 2, width() - 4, height() - 4);
        painter.drawPixmap(avatarRect, this->avatarPixmap__);
    }

    painter.restore();
}


void AvatarItem::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton)
    {
        emit clicked(this);
    }
    QWidget::mousePressEvent(event);
}

void AvatarItem::enterEvent(QEnterEvent* event)
{
    update();
    QWidget::enterEvent(event);
}

void AvatarItem::leaveEvent(QEvent* event)
{
    update();
    QWidget::leaveEvent(event);
}



