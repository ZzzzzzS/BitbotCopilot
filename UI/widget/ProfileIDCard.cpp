#include "ProfileIDCard.h"
#include "QPainter"
#include "ElaTheme.h"

ProfileIDCard::ProfileIDCard(QWidget* parent, QString profileName, QString ipAddress, QString avatar)
    :ElaInteractiveCard(parent),
    m_selected(false)
{
    this->m_profileName = profileName;
    this->setTitle(profileName.split(".ini").first());
    this->setSubTitle(ipAddress);
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
    QPainter painter(this);
    painter.save();
    painter.setRenderHints(QPainter::SmoothPixmapTransform | QPainter::Antialiasing | QPainter::TextAntialiasing);
    painter.setPen(Qt::NoPen);
    painter.setBrush(this->isSelected() ? ElaThemeColor(eTheme->getThemeMode(), BasicHoverAlpha) : Qt::transparent);
    painter.drawRoundedRect(rect(), this->getBorderRadius(), this->getBorderRadius());
    painter.restore();
    ElaInteractiveCard::paintEvent(event);
}
