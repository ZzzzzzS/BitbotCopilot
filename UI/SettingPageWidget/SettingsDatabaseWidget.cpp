#include "SettingsDatabaseWidget.h"
#include "UI/widget/SettingsLineInfoWidget.h"
#include "QHBoxLayout"
#include "QVBoxLayout"
#include "QFileDialog"
#include "ElaText.h"
#include "ElaComboBox.h"

SettingsDatabaseWidget::SettingsDatabaseWidget(QWidget* parent)
    :QWidget(parent)
{
    this->title__ = new ElaText(this);
    this->title__->setTextStyle(ElaTextType::BodyStrong);
    this->title__->setText(tr("Dataviewer & database settings"));
    this->title__->setTextPixelSize(18);
    this->title__->setAlignment(Qt::AlignTop);

    QWidget* DrawHead = new QWidget(this);
    SettingsLineInfoWidget* DrawHeadInfo = new SettingsLineInfoWidget(DrawHead, tr("Database settings"), tr("Setup remote database path and local cache"), QChar(ElaIconType::CabinetFiling));
    QHBoxLayout* DrawHeadLayout = new QHBoxLayout(DrawHead);
    DrawHeadLayout->addWidget(DrawHeadInfo);
    DrawHeadLayout->addSpacerItem(new QSpacerItem(10, 10, QSizePolicy::Expanding, QSizePolicy::Fixed));

    QWidget* DrawRemotePath = new QWidget(this);
    DrawRemotePath->setFixedHeight(60);
    SettingsLineInfoWidget* DrawRemotePathInfo = new SettingsLineInfoWidget(DrawRemotePath, tr("Remote database"));
    DrawRemotePathInfo->setTitleFontSize(14);
    this->remoteDataPath__ = new ElaLineEdit(DrawRemotePath);
    this->remoteDataPath__->setAlignment(Qt::AlignCenter);
    this->remoteDataPath__->setFixedSize(180, 30);
    this->remoteDataPath__->setPlaceholderText("/home");
    QHBoxLayout* DrawRemotePathLayout = new QHBoxLayout(DrawRemotePath);
    DrawRemotePathLayout->addWidget(DrawRemotePathInfo);
    DrawRemotePathLayout->addSpacerItem(new QSpacerItem(10, 10, QSizePolicy::Expanding, QSizePolicy::Fixed));
    DrawRemotePathLayout->addWidget(this->remoteDataPath__);
    DrawRemotePathLayout->addSpacerItem(new QSpacerItem(16, 10, QSizePolicy::Fixed, QSizePolicy::Fixed));

    QWidget* DrawLocalCachePath = new QWidget(this);
    DrawLocalCachePath->setFixedHeight(60);
    SettingsLineInfoWidget* DrawLocalCachePathInfo = new SettingsLineInfoWidget(DrawLocalCachePath, tr("Local cache path"));
    DrawLocalCachePathInfo->setTitleFontSize(14);
    this->localCachePath__ = new ElaLineEdit(DrawLocalCachePath);
    this->localCachePath__->setAlignment(Qt::AlignCenter);
    this->localCachePath__->setFixedSize(180, 30);
    this->localCachePath__->setPlaceholderText("./cache");
    this->selectLocalCachePathButton__ = new ElaPushButton(DrawLocalCachePath);
    this->selectLocalCachePathButton__->setText(tr("Select"));
    QFont btnFont = this->selectLocalCachePathButton__->font();
    btnFont.setPixelSize(14);
    this->selectLocalCachePathButton__->setFont(btnFont);
    this->selectLocalCachePathButton__->setFixedSize(120, 35);
    QHBoxLayout* DrawLocalCachePathLayout = new QHBoxLayout(DrawLocalCachePath);
    DrawLocalCachePathLayout->addWidget(DrawLocalCachePathInfo);
    DrawLocalCachePathLayout->addSpacerItem(new QSpacerItem(10, 10, QSizePolicy::Expanding, QSizePolicy::Fixed));
    DrawLocalCachePathLayout->addWidget(this->selectLocalCachePathButton__);
    DrawLocalCachePathLayout->addWidget(this->localCachePath__);
    DrawLocalCachePathLayout->addSpacerItem(new QSpacerItem(16, 10, QSizePolicy::Fixed, QSizePolicy::Fixed));

    QWidget* DrawCacheLocal = new QWidget(this);
    DrawCacheLocal->setFixedHeight(60);
    SettingsLineInfoWidget* CachelocalInfo = new SettingsLineInfoWidget(DrawCacheLocal, tr("Enable local cache"));
    CachelocalInfo->setTitleFontSize(14);
    this->cacheDataSwitch__ = new ElaToggleSwitch(DrawCacheLocal);

    QHBoxLayout* layout = new QHBoxLayout(DrawCacheLocal);
    layout->addWidget(CachelocalInfo);
    layout->addSpacerItem(new QSpacerItem(10, 10, QSizePolicy::Expanding, QSizePolicy::Fixed));
    layout->addWidget(this->cacheDataSwitch__);
    layout->addSpacerItem(new QSpacerItem(16, 10, QSizePolicy::Fixed, QSizePolicy::Fixed));

    this->DrawArea__ = new ElaDrawerArea(this);
    this->DrawArea__->setDrawerHeader(DrawHead);
    this->DrawArea__->addDrawer(DrawRemotePath);
    this->DrawArea__->addDrawer(DrawLocalCachePath);
    this->DrawArea__->addDrawer(DrawCacheLocal);

    this->DrawArea__->expand();

    QVBoxLayout* MainLayout = new QVBoxLayout(this);
    MainLayout->addWidget(this->title__);
    MainLayout->addWidget(this->DrawArea__);

    QObject::connect(this->selectLocalCachePathButton__, &ElaPushButton::clicked, [this]() {
        QString dir = QFileDialog::getExistingDirectory(this, tr("Select Local Cache Directory"), this->localCachePath__->text());
        if (!dir.isEmpty()) {
            this->localCachePath__->setText(dir);
        }
        });
}

SettingsDatabaseWidget::~SettingsDatabaseWidget()
{

}

void SettingsDatabaseWidget::setRemoteDataPath(QString path)
{
    this->remoteDataPath__->setText(path);
}

void SettingsDatabaseWidget::setLocalCachePath(QString path)
{
    this->localCachePath__->setText(path);
}

void SettingsDatabaseWidget::setCacheData(bool cache)
{
    this->cacheDataSwitch__->setIsToggled(cache);
}

QString SettingsDatabaseWidget::getRemoteDataPath()
{
    return this->remoteDataPath__->text();
}

QString SettingsDatabaseWidget::getLocalCachePath()
{
    return this->localCachePath__->text();
}

bool SettingsDatabaseWidget::getCacheData()
{
    return this->cacheDataSwitch__->getIsToggled();
}
