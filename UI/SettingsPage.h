#pragma once
#include <QWidget>
#include "ElaScrollPage.h"


class SettingsPage : public ElaScrollPage
{
	Q_OBJECT

public:
	SettingsPage(QWidget* parent = nullptr);
	~SettingsPage();

private:
};