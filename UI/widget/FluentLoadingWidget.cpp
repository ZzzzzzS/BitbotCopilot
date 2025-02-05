#include "FluentLoadingWidget.h"
#include "QVBoxLayout"
#include "QHBoxLayout"
#include "QSpacerItem"
#include "ElaTheme.h"
#include "ElaPushButton.h"
#include "QGridLayout"
#include "QApplication"

FluentLoadingWidget::FluentLoadingWidget(QWidget* parent)
	:FluentLoadingWidget(QString(),0,0,parent)
{
}

FluentLoadingWidget::FluentLoadingWidget(QString info,int min, int max, QWidget* parent)
	:QWidget(parent),
	MinRange__(min),
	MaxRange__(max)
{
	this->InfoText__ = new ElaText(info, this);
	this->InfoText__->setTextStyle(ElaTextType::Subtitle);
	this->InfoText__->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
	this->InfoText__->setAlignment(Qt::AlignLeft | Qt::AlignTop);
	this->InfoText__->setMinimumHeight(40);
	
	this->LoadingText__ = new ElaText(this);
	QFont LoadingFont = this->LoadingText__->font();
	LoadingFont.setFamily("Segoe Boot Semilight");
	LoadingFont.setPointSize(24);
	this->LoadingText__->setFont(LoadingFont);
	this->LoadingText__->setFixedSize(42, 42);
	this->LoadingText__->setAlignment(Qt::AlignCenter);

	this->PercentText__ = new ElaText(this);
	this->PercentText__->setText("0%");
	this->PercentText__->setTextStyle(ElaTextType::Body);
	this->PercentText__->setAlignment(Qt::AlignHCenter|Qt::AlignBottom);
	this->PercentText__->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Policy::Minimum);

	QVBoxLayout* LoadingVLay = new QVBoxLayout();
	LoadingVLay->addWidget(this->LoadingText__);
	LoadingVLay->addWidget(this->PercentText__);

	QHBoxLayout* LoadingHLay = new QHBoxLayout();
	QSpacerItem* LoadingLeftSpace = new QSpacerItem(20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
	QSpacerItem* LoadingRightSpace = new QSpacerItem(20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
	LoadingHLay->addSpacerItem(LoadingLeftSpace);
	LoadingHLay->addLayout(LoadingVLay);
	LoadingHLay->addSpacerItem(LoadingRightSpace);

	QVBoxLayout* CenterVLay = new QVBoxLayout(this);
	QSpacerItem* InfoLoadingSpace = new QSpacerItem(20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding);
	CenterVLay->addWidget(this->InfoText__);
	CenterVLay->addSpacerItem(InfoLoadingSpace);
	CenterVLay->addLayout(LoadingHLay);

	if (this->MinRange__ == 0 && this->MaxRange__ == 0)
	{
		this->PercentText__->hide();
	}

	this->setContentsMargins(15, 20, 15, 0);

	this->ThemeChanged(eTheme->getThemeMode());
	QObject::connect(eTheme, &ElaTheme::themeModeChanged, this, &FluentLoadingWidget::ThemeChanged);

	this->RefreshTimer__ = new QTimer(this);
	this->RefreshTimer__->setInterval(30);
	QObject::connect(this->RefreshTimer__, &QTimer::timeout, this, &FluentLoadingWidget::RefreshLoadingAnimation);
	this->RefreshTimer__->start();
	this->LoadingAnimationCounter__ = this->BEGIN_CHARACTER_INDEX__;
}

FluentLoadingWidget::~FluentLoadingWidget()
{
	qDebug() << "good bye new loading";
}

void FluentLoadingWidget::UpdatePercentage(int p)
{
	if (this->MinRange__ == 0 && this->MaxRange__ == 0)
		return;

	double percent = static_cast<double>(p) / static_cast<double>(this->MaxRange__ - this->MinRange__) * 100;
	QString percentText = QString::number(percent, 'g', 2);
	this->PercentText__->setText(percentText + "%");
}

void FluentLoadingWidget::UpdateInfo(QString info)
{
	this->InfoText__->setText(info);
}

void FluentLoadingWidget::RefreshLoadingAnimation()
{
	unsigned short loadingIdx = (this->LoadingAnimationCounter__ > this->END_CHARACTER_INDEX__) ? this->END_CHARACTER_INDEX__ : this->LoadingAnimationCounter__;
	this->LoadingText__->setText(QChar(loadingIdx));
	this->LoadingAnimationCounter__++;
	if (this->LoadingAnimationCounter__ > this->END_CHARACTER_INDEX__+this->END_SLEEP_OFFSET)
		this->LoadingAnimationCounter__ = this->BEGIN_CHARACTER_INDEX__;
}

void FluentLoadingWidget::ThemeChanged(ElaThemeType::ThemeMode theme)
{
	QColor LoadingColor;
	if (theme == ElaThemeType::Light)
	{
		LoadingColor = eTheme->getThemeColor(theme, ElaThemeType::PrimaryNormal);
	}
	else
	{
		LoadingColor = eTheme->getThemeColor(theme, ElaThemeType::PrimaryNormal);
	}
	QPalette LoadingPalette;
	qDebug() << LoadingColor;
	QBrush brush_loadingLabel(LoadingColor);
	brush_loadingLabel.setStyle(Qt::SolidPattern);
	LoadingPalette.setBrush(QPalette::Active, QPalette::WindowText, brush_loadingLabel);
	LoadingPalette.setBrush(QPalette::Inactive, QPalette::WindowText, brush_loadingLabel);
	this->LoadingText__->setPalette(LoadingPalette);
}

FluentProgressDialog::FluentProgressDialog(const QString& labelText, const QString& cancelButtonText, int minimum, int maximum, QWidget* parent)
	:ElaContentDialog(parent)
{
	this->LoadingWidget = new FluentLoadingWidget(labelText, minimum, maximum, this);
	this->setCentralWidget(this->LoadingWidget);
	this->setButtonNumber(1);
	this->setRightButtonText(cancelButtonText, true);
	QObject::connect(this, &FluentProgressDialog::rightButtonClicked, this, [this]() {
		emit this->canceled();
	});
}

FluentProgressDialog::~FluentProgressDialog()
{
	qDebug() << "fluent diag good bye";
}

void FluentProgressDialog::UpdatePercentage(int p)
{
	this->LoadingWidget->UpdatePercentage(p);
}

void FluentProgressDialog::UpdateInfo(QString info)
{
	this->LoadingWidget->UpdateInfo(info);
}

SimpleInfinateLoadingWidget::SimpleInfinateLoadingWidget(QWidget* parent)
	:SimpleInfinateLoadingWidget(12, parent)
{
}

SimpleInfinateLoadingWidget::SimpleInfinateLoadingWidget(size_t FontSize, QWidget* parent)
	:ElaText(parent)
{
	this->LoadingText__ = this;
	QFont LoadingFont = this->LoadingText__->font();
	LoadingFont.setFamily("Segoe Boot Semilight");
	LoadingFont.setPointSize(FontSize);
	this->LoadingText__->setFont(LoadingFont);
	this->LoadingText__->setAlignment(Qt::AlignCenter);
	this->RefreshTimer__ = new QTimer(this);
	this->RefreshTimer__->setInterval(30);
	QObject::connect(this->RefreshTimer__, &QTimer::timeout, this, &SimpleInfinateLoadingWidget::RefreshLoadingAnimation);
	this->RefreshTimer__->start();
	this->LoadingAnimationCounter__ = this->BEGIN_CHARACTER_INDEX__;
	this->ThemeChanged(eTheme->getThemeMode());
	QObject::connect(eTheme, &ElaTheme::themeModeChanged, this, &SimpleInfinateLoadingWidget::ThemeChanged);
	this->hide();
	/*QHBoxLayout* layout = new QHBoxLayout(this);
	layout->addWidget(this->LoadingText__);*/
}

SimpleInfinateLoadingWidget::~SimpleInfinateLoadingWidget()
{
}

void SimpleInfinateLoadingWidget::RefreshLoadingAnimation()
{
	unsigned short loadingIdx = (this->LoadingAnimationCounter__ > this->END_CHARACTER_INDEX__) ? this->END_CHARACTER_INDEX__ : this->LoadingAnimationCounter__;
	this->LoadingText__->setText(QChar(loadingIdx));
	this->LoadingAnimationCounter__++;
	if (this->LoadingAnimationCounter__ > this->END_CHARACTER_INDEX__ + this->END_SLEEP_OFFSET)
		this->LoadingAnimationCounter__ = this->BEGIN_CHARACTER_INDEX__;
}

void SimpleInfinateLoadingWidget::ThemeChanged(ElaThemeType::ThemeMode theme)
{
	QColor LoadingColor;
	if (theme == ElaThemeType::Light)
	{
		LoadingColor = eTheme->getThemeColor(theme, ElaThemeType::PrimaryNormal);
	}
	else
	{
		LoadingColor = eTheme->getThemeColor(theme, ElaThemeType::PrimaryNormal);
	}
	QPalette LoadingPalette;
	qDebug() << LoadingColor;
	QBrush brush_loadingLabel(LoadingColor);
	brush_loadingLabel.setStyle(Qt::SolidPattern);
	LoadingPalette.setBrush(QPalette::Active, QPalette::WindowText, brush_loadingLabel);
	LoadingPalette.setBrush(QPalette::Inactive, QPalette::WindowText, brush_loadingLabel);
	this->LoadingText__->setPalette(LoadingPalette);
}

void SimpleInfinateLoadingWidget::start(bool start)
{
	if (start)
	{
		this->RefreshTimer__->start();
		this->LoadingAnimationCounter__ = this->BEGIN_CHARACTER_INDEX__;
		this->RefreshLoadingAnimation();
		qApp->processEvents();
		this->show();
	}
	else
	{
		this->RefreshTimer__->stop();
		this->hide();
	}
}
