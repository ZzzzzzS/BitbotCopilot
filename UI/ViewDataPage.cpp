#include "ViewDataPage.h"
#include "QVBoxLayout"
#include "DataViewer/DataViewerPage.h"
#include "ElaTabWidget.h"
#include "ElaPushButton.h"

ViewDataPage::ViewDataPage(QWidget *parent)
	: ElaScrollPage(parent)
{
	this->setPageTitleSpacing(10);
	auto data1 = new DataViewerPage(this);
	QVBoxLayout* centerVLayout = new QVBoxLayout(data1);
	centerVLayout->setContentsMargins(0, 0, 0, 0);
	this->addCentralWidget(data1, true, false, 0);
}

ViewDataPage::~ViewDataPage()
{
}
