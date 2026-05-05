#include "SettingsAutorunLauncher.h"
#include "UI/widget/SettingsLineInfoWidget.h"
#include "UI/widget/ClickableElaScrollPageArea.h"
#include "QHBoxLayout"
#include "QVBoxLayout"
#include "QDebug"
#include "SettingsAutorunCfgWidget.h"

SettingsAutorunLauncher::SettingsAutorunLauncher(QWidget* parent)
    :QWidget(parent)
{
    this->title__ = new ElaText(this);
    this->title__->setTextStyle(ElaTextType::BodyStrong);
    this->title__->setText(tr("Auto initialization robot"));
    this->title__->setTextPixelSize(18);
    this->title__->setAlignment(Qt::AlignTop);

    ClickableElaScrollPageArea* AutoRunArea = new ClickableElaScrollPageArea(this);
    SettingsLineInfoWidget* AutoRunAreaInfo = new SettingsLineInfoWidget(AutoRunArea, tr("Config autorun commands"), tr("add or update autorun commands during robot initialzation"), QChar(ElaIconType::ListCheck));
    this->OpenSettings__ = new ElaText(AutoRunArea);
    this->OpenSettings__->setText(QChar(ElaIconType::ArrowUpRightFromSquare));
    QFont iconFont = this->OpenSettings__->font();
    iconFont.setFamily("ElaAwesome");
    iconFont.setPixelSize(14);
    this->OpenSettings__->setFixedSize(40, 40);
    this->OpenSettings__->setFont(iconFont);
    this->OpenSettings__->setAlignment(Qt::AlignCenter);

    QHBoxLayout* layout = new QHBoxLayout(AutoRunArea);
    layout->addWidget(AutoRunAreaInfo);
    layout->addSpacerItem(new QSpacerItem(10, 10, QSizePolicy::Expanding, QSizePolicy::Fixed));
    layout->addWidget(this->OpenSettings__);
    layout->addSpacerItem(new QSpacerItem(16, 10, QSizePolicy::Fixed, QSizePolicy::Fixed));


    QVBoxLayout* MainLayout = new QVBoxLayout(this);
    MainLayout->addWidget(this->title__);
    MainLayout->addWidget(AutoRunArea);

    QObject::connect(AutoRunArea, &ClickableElaScrollPageArea::clicked, this, &SettingsAutorunLauncher::OpenSettingsSlot);
}

SettingsAutorunLauncher::~SettingsAutorunLauncher()
{

}

AutoRunCmdList SettingsAutorunLauncher::getCmdList()
{
    if (this->cmdList__.has_value())
    {
        return this->cmdList__.value();
    }
    else
    {
        return AutoRunCmdList();
    }
}

void SettingsAutorunLauncher::setCmdList(const AutoRunCmdList& list)
{
    this->cmdList__ = list;
}

void SettingsAutorunLauncher::OpenSettingsSlot()
{
    auto widget = new SettingsAutorunCfgWidget(nullptr);
    this->cmdList__ = widget->exec();
}
