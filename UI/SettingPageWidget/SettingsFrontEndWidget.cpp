#include "SettingsFrontEndWidget.h"
#include "UI/widget/SettingsLineInfoWidget.h"
#include "QHBoxLayout"
#include "QVBoxLayout"

SettingsFrontEndWidget::SettingsFrontEndWidget(QWidget* parent)
    :QWidget(parent)
{
    this->title__ = new ElaText(this);
    this->title__->setTextStyle(ElaTextType::BodyStrong);
    this->title__->setText(tr("Frontend manager"));
    this->title__->setTextPixelSize(18);
    this->title__->setAlignment(Qt::AlignTop);

    QWidget* DrawHead = new QWidget(this);
    SettingsLineInfoWidget* DrawHeadInfo = new SettingsLineInfoWidget(DrawHead, tr("Frontend address"), tr("Setup frontend ip address and port"), QChar(ElaIconType::Computer));
    QHBoxLayout* DrawHeadLayout = new QHBoxLayout(DrawHead);
    DrawHeadLayout->addWidget(DrawHeadInfo);
    DrawHeadLayout->addSpacerItem(new QSpacerItem(10, 10, QSizePolicy::Expanding, QSizePolicy::Fixed));

    QWidget* DrawIP = new QWidget(this);
    DrawIP->setFixedHeight(60);
    SettingsLineInfoWidget* DrawIPInfo = new SettingsLineInfoWidget(DrawIP, tr("Frontend ip Address"));
    DrawIPInfo->setTitleFontSize(14);
    this->ipAddress__ = new ElaLineEdit(DrawIP);
    this->ipAddress__->setAlignment(Qt::AlignCenter);
    this->ipAddress__->setFixedSize(180, 30);
    this->ipAddress__->setText("localhost");
    QHBoxLayout* DrawIPLayout = new QHBoxLayout(DrawIP);
    DrawIPLayout->addWidget(DrawIPInfo);
    DrawIPLayout->addSpacerItem(new QSpacerItem(10, 10, QSizePolicy::Expanding, QSizePolicy::Fixed));
    DrawIPLayout->addWidget(this->ipAddress__);
    DrawIPLayout->addSpacerItem(new QSpacerItem(16, 10, QSizePolicy::Fixed, QSizePolicy::Fixed));

    QWidget* DrawPort = new QWidget(this);
    DrawPort->setFixedHeight(60);
    SettingsLineInfoWidget* DrawPortInfo = new SettingsLineInfoWidget(DrawPort, tr("Frontend port"));
    DrawPortInfo->setTitleFontSize(14);
    this->portBox__ = new ElaSpinBox(DrawPort);
    this->portBox__->setRange(1, 65535);
    this->portBox__->setValue(12888);
    this->portBox__->setFixedWidth(120);
    QHBoxLayout* DrawPortLayout = new QHBoxLayout(DrawPort);
    DrawPortLayout->addWidget(DrawPortInfo);
    DrawPortLayout->addSpacerItem(new QSpacerItem(10, 10, QSizePolicy::Expanding, QSizePolicy::Fixed));
    DrawPortLayout->addWidget(this->portBox__);
    DrawPortLayout->addSpacerItem(new QSpacerItem(16, 10, QSizePolicy::Fixed, QSizePolicy::Fixed));

    this->DrawArea__ = new ElaDrawerArea(this);
    this->DrawArea__->setDrawerHeader(DrawHead);
    this->DrawArea__->addDrawer(DrawIP);
    this->DrawArea__->addDrawer(DrawPort);
    this->DrawArea__->expand();

    QVBoxLayout* MainLayout = new QVBoxLayout(this);
    MainLayout->addWidget(this->title__);
    MainLayout->addWidget(this->DrawArea__);
}

SettingsFrontEndWidget::~SettingsFrontEndWidget()
{

}

void SettingsFrontEndWidget::setIP(QString ip)
{
    this->ipAddress__->setText(ip);
}

void SettingsFrontEndWidget::setPort(uint16_t port)
{
    this->portBox__->setValue(port);
}

QString SettingsFrontEndWidget::getIP()
{
    return this->ipAddress__->text();
}

uint16_t SettingsFrontEndWidget::getPort()
{
    return this->portBox__->value();
}
