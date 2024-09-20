#include "ViewDataPage.h"
#include "QVBoxLayout"
#include "DataViewer/DataViewerPage.h"

ViewDataPage::ViewDataPage(QWidget *parent)
	: ElaScrollPage(parent)
	, ui(new Ui::ViewDataPageClass())
{
	//ui->setupUi(this);

	QWidget* centralWidget = new DataViewerPage(this);
	centralWidget->setWindowTitle(tr(" Data Viewer"));
	this->setPageTitleSpacing(10);
	QVBoxLayout* centerVLayout = new QVBoxLayout(centralWidget);
	centerVLayout->setContentsMargins(0, 0, 0, 0);

	this->addCentralWidget(centralWidget, true, true, 0); 
}

ViewDataPage::~ViewDataPage()
{
	//delete ui;
}
