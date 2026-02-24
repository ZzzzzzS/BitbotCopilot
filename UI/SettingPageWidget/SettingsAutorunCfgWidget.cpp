#include "SettingsAutorunCfgWidget.h"
#include "QApplication"
#include "qeventloop.h"
#include "QVBoxLayout"
#include "QHBoxLayout"
#include "ElaTheme.h"
#include "UI/widget/FluentMessageBox.hpp"

SettingsAutorunCfgWidget::SettingsAutorunCfgWidget(QWidget* parent)
    :ElaWidget(parent)
{
    this->setAttribute(Qt::WA_DeleteOnClose);
    //this->setWindowTitle(tr("Config Autorun"));
    this->resize(1200, 720);
    this->setWindowModality(Qt::WindowModality::ApplicationModal);
    this->InitTitle();
    this->abcdefg();

    this->InitLayout();
}

SettingsAutorunCfgWidget::~SettingsAutorunCfgWidget()
{

}

std::optional<AutoRunCmdList> SettingsAutorunCfgWidget::getCmdList()
{
    return std::optional<AutoRunCmdList>();
}

std::optional<AutoRunCmdList> SettingsAutorunCfgWidget::exec()
{
    QEventLoop loop;
    this->setWindowModality(Qt::WindowModality::ApplicationModal);
    this->show();
    std::optional<AutoRunCmdList> CmdList;

    QObject::connect(this, &SettingsAutorunCfgWidget::Selected, this, [&, this]() {
        CmdList = this->getCmdList();
        if (CmdList.has_value())
        {
            this->close();
        }
        else
        {
            FluentMessageBox::informationOk(this, tr("No commands are added"), tr("please add autorun commands or click cancel button to cancel action."));
        }
        });

    QObject::connect(this, &SettingsAutorunCfgWidget::Canceled, this, [this]() {
        this->close();
        });
    QObject::connect(this, &SettingsAutorunCfgWidget::destroyed, &loop, &QEventLoop::quit);

    loop.exec();
    return CmdList;
}

void SettingsAutorunCfgWidget::ContinueButtonClickedSlot()
{
    //TODO: process actions
    emit this->Selected();
}

void SettingsAutorunCfgWidget::InitTitle()
{
    this->Title__ = new ElaText(this);
    this->Title__->setText("Configure Autorun");

    this->CancelButton__ = new ElaPushButton(this);
    this->CancelButton__->setText(tr("Cancel"));
    this->CancelButton__->setFixedSize(120, 35);
    QObject::connect(this->CancelButton__, &ElaPushButton::clicked, this, &SettingsAutorunCfgWidget::Canceled);

    this->ContinueButton__ = new ElaPushButton(this);
    this->ContinueButton__->setText(tr("OK"));
    this->ContinueButton__->setFixedSize(120, 35);
    QColor ButtonLightDefaultColor(eTheme->getThemeColor(ElaThemeType::ThemeMode::Light, ElaThemeType::ThemeColor::PrimaryNormal));
    QColor ButtonLightHoverColor(eTheme->getThemeColor(ElaThemeType::ThemeMode::Light, ElaThemeType::ThemeColor::PrimaryHover));
    QColor ButtonLightPressColor(eTheme->getThemeColor(ElaThemeType::ThemeMode::Light, ElaThemeType::ThemeColor::PrimaryPress));
    QColor ButtonLightTextColor(255, 255, 255);
    QColor ButtonDarkDefaultColor(eTheme->getThemeColor(ElaThemeType::ThemeMode::Dark, ElaThemeType::ThemeColor::PrimaryNormal));
    QColor ButtonDarkHoverColor(eTheme->getThemeColor(ElaThemeType::ThemeMode::Dark, ElaThemeType::ThemeColor::PrimaryHover));
    QColor ButtonDarkPressColor(eTheme->getThemeColor(ElaThemeType::ThemeMode::Dark, ElaThemeType::ThemeColor::PrimaryPress));
    QColor ButtonDarkTextColor(0, 0, 0);
    this->ContinueButton__->setDarkDefaultColor(ButtonDarkDefaultColor);
    this->ContinueButton__->setDarkHoverColor(ButtonDarkHoverColor);
    this->ContinueButton__->setDarkPressColor(ButtonDarkPressColor);
    this->ContinueButton__->setDarkTextColor(ButtonDarkTextColor);
    this->ContinueButton__->setLightDefaultColor(ButtonLightDefaultColor);
    this->ContinueButton__->setLightHoverColor(ButtonLightHoverColor);
    this->ContinueButton__->setLightPressColor(ButtonLightPressColor);
    this->ContinueButton__->setLightTextColor(ButtonLightTextColor);
    QObject::connect(this->ContinueButton__, &ElaPushButton::clicked, this, &SettingsAutorunCfgWidget::ContinueButtonClickedSlot);

    QString DescriptionString = tr("add the commands you want to execute automaticlly during robot startup, and configure their order, execution interval or conditions.");
    this->Description__ = new ElaText(this);
    this->Description__->setTextStyle(ElaTextType::TextStyle::BodyStrong);
    this->Description__->setText(DescriptionString);
}

void SettingsAutorunCfgWidget::abcdefg()
{
    //这个函数名是xxx提供，感谢xxx喵，谢谢喵~

}

void SettingsAutorunCfgWidget::InitLayout()
{
    QVBoxLayout* GlobalLayout = new QVBoxLayout(this);

    QHBoxLayout* TitleLayout = new QHBoxLayout(this);
    TitleLayout->addWidget(this->Title__);
    TitleLayout->addSpacerItem(new QSpacerItem(10, 10, QSizePolicy::Expanding, QSizePolicy::Minimum));
    TitleLayout->addWidget(this->ContinueButton__);
    TitleLayout->addWidget(this->CancelButton__);
    QVBoxLayout* TitleDescLayout = new QVBoxLayout(this);
    TitleDescLayout->addLayout(TitleLayout);
    TitleDescLayout->addSpacerItem(new QSpacerItem(10, 20, QSizePolicy::Minimum, QSizePolicy::Fixed));
    TitleDescLayout->addWidget(this->Description__);
    TitleDescLayout->addSpacerItem(new QSpacerItem(10, 20, QSizePolicy::Minimum, QSizePolicy::Fixed));

    GlobalLayout->addLayout(TitleDescLayout);
    GlobalLayout->addSpacerItem(new QSpacerItem(10, 10, QSizePolicy::Minimum, QSizePolicy::Expanding));
}


AutorunLineCfg::AutorunLineCfg(QWidget* parent)
{

}

AutorunLineCfg::~AutorunLineCfg()
{

}

AutoRunCommand_t AutorunLineCfg::getLineCfg()
{
    return AutoRunCommand_t();
}

void AutorunLineCfg::setLineCfg(AutoRunCommand_t& cfg)
{

}
