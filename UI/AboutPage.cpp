#include "AboutPage.h"
#include "QVBoxLayout"
#include "ui_AboutPage.h"
#include "QDesktopServices"
#include "QDate"
#include "QMessageBox"
#include "QFile"
#include "ElaScrollArea.h"
#include <QHBoxLayout>
#include <QUrl>
#include <QDebug>
#include "QSimpleUpdater.h"
#include "../Utils/Settings/SettingsHandler.h"
#include "UI/widget/FluentMessageBox.hpp"

AboutPage::AboutPage(QWidget* parent)
{
	AboutPageCentralWidget* centralWidget = new AboutPageCentralWidget(this);
	centralWidget->setWindowTitle(tr("About"));
	QVBoxLayout* centerVLayout = new QVBoxLayout();
	centerVLayout->setContentsMargins(0, 0, 0, 0);

	this->addCentralWidget(centralWidget, true, true, 0);
}

AboutPage::~AboutPage()
{
}

AboutPageCentralWidget::AboutPageCentralWidget(QWidget* parent)
	:QWidget(parent),
	ui(new Ui::AboutPageCentralWidget)
{
	ui->setupUi(this);
	this->resize(380, 500);



	QString InfoText = this->ui->VersionLabel->text();

	QString dateTime = __DATE__;
	dateTime.replace(" ", "");
	QDate BuildDate = QLocale(QLocale::English).toDate(dateTime, "MMMdyyyy");
	qDebug() << BuildDate;
	InfoText += BuildDate.toString(Qt::ISODate) + "-";
	InfoText += QString(BUILD_VERSION_COMMIT_HASH);
	this->ui->VersionLabel->setText(InfoText);

	QObject::connect(this->ui->QtButton, &QPushButton::clicked, qApp, &QApplication::aboutQt);
	QObject::connect(this->ui->SoftwareLicenseButton, &QPushButton::clicked, this, []() {
		QDesktopServices::openUrl(QUrl("https://github.com/ZzzzzzS/BitbotCopilot/blob/main/LICENSE"));
		});
	QObject::connect(this->ui->BitbotButton, &QPushButton::clicked, this, []() {
		QDesktopServices::openUrl(QUrl("https://bitbot.lmy.name/"));
		});
	QObject::connect(this->ui->FeedBackButton, &QPushButton::clicked, this, []() {
		QDesktopServices::openUrl(QUrl("https://github.com/ZzzzzzS/BitbotCopilot/issues"));
		});
	QObject::connect(this->ui->ThirdpartyButton, &QPushButton::clicked, this, &AboutPageCentralWidget::ThirdpartyLicenseSlot);
	this->InitUpdate();
	QObject::connect(eTheme, &ElaTheme::themeModeChanged, this, &AboutPageCentralWidget::ThemeChangedSlot);
	this->ThemeChangedSlot(eTheme->getThemeMode());
	//this->setIsFixedSize(true);
	//this->setIsStayTop(false);
	this->setWindowModality(Qt::ApplicationModal);
}

AboutPageCentralWidget::~AboutPageCentralWidget()
{
}

void AboutPageCentralWidget::ThirdpartyLicenseSlot()
{
	auto ptr = new AboutPageLicenseWidget();
	ptr->setAttribute(Qt::WA_DeleteOnClose);
	ptr->setIsFixedSize(true);
	ptr->setWindowModality(Qt::ApplicationModal);
	ptr->show();
}

void AboutPageCentralWidget::CheckUpdateSlot()
{
	bool BetaVer = ZSet->isUpdateBetaChannel();
	if (BetaVer)
	{
		FluentMessageBox::informationOk(this, tr("Beta Channel"), tr("You are using the beta channel, the update may be unstable"));
	}

	this->ui->UpdateLoadingWidget->start(true);
	this->ui->UpdatesButton->setEnabled(false);
	QString UpdateURL = BetaVer ? BETA_UPDATE_URL : UPDATE_URL;
	updater->setModuleVersion(UpdateURL, BUILD_VERSION_COMMIT_HASH);
	updater->setDownloadDir(UpdateURL, "./");
	//updater->setModuleVersion(UpdateURL, "0433526");
	updater->setNotifyOnFinish(UpdateURL, true);
	updater->setNotifyOnUpdate(UpdateURL, true);
	updater->setUseCustomAppcast(UpdateURL, false);
	updater->setDownloaderEnabled(UpdateURL, true);
	updater->setMandatoryUpdate(UpdateURL, false);

	/* Check for updates */
	updater->checkForUpdates(UpdateURL);
}

void AboutPageCentralWidget::InitUpdate()
{
	this->updater = QSimpleUpdater::getInstance();
	QObject::connect(this->ui->UpdatesButton, &ElaPushButton::clicked, this, &AboutPageCentralWidget::CheckUpdateSlot);
	QObject::connect(this->updater, &QSimpleUpdater::appcastDownloaded, this, [this](const QString& url, const QByteArray& data) {
		qDebug() << "appcastDownloaded";
		});
	QObject::connect(this->updater, &QSimpleUpdater::checkingFinished, this, [this](const QString& url) {
		qDebug() << "checkingFinished";
		this->ui->UpdateLoadingWidget->start(false);
		this->ui->UpdatesButton->setEnabled(true);
		});

	QObject::connect(this->updater, &QSimpleUpdater::downloadFinished, this, [this](const QString& url, const QString& filepath) {
		qDebug() << "downloadFinished";
		});
}

void AboutPageCentralWidget::ThemeChangedSlot(ElaThemeType::ThemeMode theme)
{
	QPalette palette1;
	QPalette palette_versionLabel;
	if (theme == ElaThemeType::Dark)
	{
		QBrush brush2(QColor(152, 234, 254, 255));
		brush2.setStyle(Qt::SolidPattern);
		palette1.setBrush(QPalette::Active, QPalette::ButtonText, brush2);
		palette1.setBrush(QPalette::Inactive, QPalette::ButtonText, brush2);

		
		QBrush brush_versionLabel(QColor(206, 206, 206, 255));
		brush_versionLabel.setStyle(Qt::SolidPattern);
		palette_versionLabel.setBrush(QPalette::Active, QPalette::WindowText, brush_versionLabel);
		palette_versionLabel.setBrush(QPalette::Inactive, QPalette::WindowText, brush_versionLabel);
	}
	else
	{	
		QBrush brush2(QColor(1, 63, 146, 255));
		brush2.setStyle(Qt::SolidPattern);
		palette1.setBrush(QPalette::Active, QPalette::ButtonText, brush2);
		palette1.setBrush(QPalette::Inactive, QPalette::ButtonText, brush2);

		QBrush brush_versionLabel(QColor(55, 55, 55, 255));
		brush_versionLabel.setStyle(Qt::SolidPattern);
		palette_versionLabel.setBrush(QPalette::Active, QPalette::WindowText, brush_versionLabel);
		palette_versionLabel.setBrush(QPalette::Inactive, QPalette::WindowText, brush_versionLabel);
	}

	this->ui->QtButton->setPalette(palette1);
	this->ui->BitbotButton->setPalette(palette1);
	this->ui->SoftwareLicenseButton->setPalette(palette1);
	this->ui->ThirdpartyButton->setPalette(palette1);
	this->ui->VersionLabel->setPalette(palette_versionLabel);
}

AboutPageLicenseWidget::AboutPageLicenseWidget()
	:ElaWidget()
{
	this->setWindowTitle(tr("Acknowledgment"));
	ElaScrollArea* CencralScroll = new ElaScrollArea(this);
	CencralScroll->setIsGrabGesture(true,0);
	CencralScroll->setWidgetResizable(true);
	auto scrollAreaWidgetContents = new QWidget();
	scrollAreaWidgetContents->setStyleSheet(R"(background-color:rgba(0,0,0,0);)");
	auto gridLayout = new QGridLayout(scrollAreaWidgetContents);

	ElaText* LicenseText = new ElaText(scrollAreaWidgetContents);
	gridLayout->addWidget(LicenseText, 0, 0, 1, 1);
	CencralScroll->setWidget(scrollAreaWidgetContents);

	QString seperater= QString("\n\n------------------------------------------------------------------\n");

	QString LicenseTextString=QString("Third-Party Software Acknowledgment");
	LicenseTextString += seperater;

	//ElaWidgetTools
	LicenseTextString += QString("ElaWidgetTools\n");
	LicenseTextString += QString("https://github.com/Liniyous/ElaWidgetTools\n\n");
	QFile Elawidget_license;
	Elawidget_license.setFileName(":/license/licenses/LICENSE-ElaWidget");
	Elawidget_license.open(QIODevice::ReadOnly);
	QTextStream Elawidget_buffer(&Elawidget_license);
	LicenseTextString += Elawidget_buffer.readAll();
	LicenseTextString += seperater;

	//QCustomPlot
	LicenseTextString += QString("QCustomPlot\n");
	LicenseTextString += QString("https://www.qcustomplot.com/\n\n");
	QFile QCP_license;
	QCP_license.setFileName(":/license/licenses/LICENSE-QCustomPlot");
	QCP_license.open(QIODevice::ReadOnly);
	QTextStream QCP_buffer(&QCP_license);
	LicenseTextString += QCP_buffer.readAll();
	LicenseTextString += seperater;

	//QSimpleUpdater
	LicenseTextString += QString("QSimpleUpdater\n");
	LicenseTextString += QString("https://github.com/alex-spataru/QSimpleUpdater\n\n");
	QFile QSU_license;
	QSU_license.setFileName(":/license/licenses/LICENSE-QSimpleUpdater");
	QSU_license.open(QIODevice::ReadOnly);
	QTextStream QSU_buffer(&QSU_license);
	LicenseTextString += QSU_buffer.readAll();
	LicenseTextString += seperater;


	LicenseText->setText(LicenseTextString);
	QFont font;
	font.setPointSizeF(10);
	LicenseText->setFont(font);
	ElaPushButton* ok_button = new ElaPushButton(this);
	QSpacerItem* sp = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
	QSpacerItem* sp2 = new QSpacerItem(20, 20, QSizePolicy::Minimum, QSizePolicy::Minimum);
	QHBoxLayout* h_lay = new QHBoxLayout();
	h_lay->addSpacerItem(sp);
	h_lay->addWidget(ok_button);
	h_lay->addSpacerItem(sp2);

	ok_button->setText(tr("OK"));
	ok_button->setMaximumWidth(100);
	ok_button->setMinimumWidth(80);
	QObject::connect(ok_button, &QPushButton::clicked, this, &AboutPageLicenseWidget::close);

	QVBoxLayout* lay = new QVBoxLayout(this);
	lay->addWidget(CencralScroll);
	lay->addLayout(h_lay);

}

AboutPageLicenseWidget::~AboutPageLicenseWidget()
{
}
