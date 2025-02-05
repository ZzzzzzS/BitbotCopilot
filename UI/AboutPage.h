#pragma once
#include <QWidget>
#include "ElaScrollPage.h"
#include "ElaWidget.h"
#include "ElaTheme.h"

class QSimpleUpdater;

class AboutPage : public ElaScrollPage
{
	Q_OBJECT

public:
	AboutPage(QWidget* parent = nullptr);
	~AboutPage();

private:
};

namespace Ui {
	class AboutPageCentralWidget;
}

class AboutPageCentralWidget :public QWidget
{
	Q_OBJECT
public:
	explicit AboutPageCentralWidget(QWidget* parent = 0);
	~AboutPageCentralWidget();

private:
	Ui::AboutPageCentralWidget* ui;
	QSimpleUpdater* updater;
private:
	void ThirdpartyLicenseSlot();
	void CheckUpdateSlot();
	void InitUpdate();
	void ThemeChangedSlot(ElaThemeType::ThemeMode theme);

};

class AboutPageLicenseWidget : public ElaWidget
{
	Q_OBJECT
public:
	explicit AboutPageLicenseWidget();
	~AboutPageLicenseWidget();
};