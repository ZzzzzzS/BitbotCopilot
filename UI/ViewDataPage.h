#pragma once

#include <QWidget>
#include "ElaScrollPage.h"
#include "DataViewer/DataViewerPage.h"


class ViewDataPage : public ElaScrollPage
{
	Q_OBJECT

public:
	ViewDataPage(QWidget *parent = nullptr);
	~ViewDataPage();
};
