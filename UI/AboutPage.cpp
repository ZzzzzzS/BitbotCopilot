#include "AboutPage.h"
#include "QVBoxLayout"

AboutPage::AboutPage(QWidget* parent)
{
	QWidget* centralWidget = new QWidget(this);
	centralWidget->setWindowTitle("About Bitbot Copilot");
	QVBoxLayout* centerVLayout = new QVBoxLayout(centralWidget);
	centerVLayout->setContentsMargins(0, 0, 0, 0);

	this->addCentralWidget(centralWidget, true, true, 0);
}

AboutPage::~AboutPage()
{
}
