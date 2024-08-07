#pragma once

#include <QWidget>
#include "ui_ViewDataPage.h"
#include "ElaScrollPage.h"

QT_BEGIN_NAMESPACE
namespace Ui { class ViewDataPageClass; };
QT_END_NAMESPACE

class ViewDataPage : public ElaScrollPage
{
	Q_OBJECT

public:
	ViewDataPage(QWidget *parent = nullptr);
	~ViewDataPage();

private:
	Ui::ViewDataPageClass *ui;
};
