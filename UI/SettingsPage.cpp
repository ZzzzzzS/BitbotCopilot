#include "SettingsPage.h"
#include "QVBoxLayout"

SettingsPage::SettingsPage(QWidget* parent)
{
	QWidget* centralWidget = new QWidget(this);
	centralWidget->setWindowTitle(tr("Settings"));
	QVBoxLayout* centerVLayout = new QVBoxLayout(centralWidget);
	centerVLayout->setContentsMargins(0, 0, 0, 0);

	this->addCentralWidget(centralWidget, true, true, 0);
}

SettingsPage::~SettingsPage()
{
}
