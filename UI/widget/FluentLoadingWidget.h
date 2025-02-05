#pragma once
#include <QWidget>
#include "ElaText.h"
#include "QTimer"
#include "ElaContentDialog.h"



#define USE_WINDOWS10_STYLE //otherwise use windows 11 style which I don't like it
class FluentLoadingWidget : public QWidget
{
	Q_OBJECT
public:
	FluentLoadingWidget(QWidget* parent = nullptr);
	FluentLoadingWidget(QString info, int min, int max, QWidget* parent = nullptr);
	~FluentLoadingWidget();
	void UpdatePercentage(int p);
	void UpdateInfo(QString info);
private:
	void RefreshLoadingAnimation();
	void ThemeChanged(ElaThemeType::ThemeMode theme);
private:
	QTimer* RefreshTimer__;
	int MinRange__;
	int MaxRange__;
	ElaText* InfoText__;
	ElaText* PercentText__;
	ElaText* LoadingText__;

	size_t LoadingAnimationCounter__;
#ifdef USE_WINDOWS10_STYLE
	const size_t BEGIN_CHARACTER_INDEX__ = 0xE052;
	const size_t END_CHARACTER_INDEX__ = 0xE0CB;
	const size_t END_SLEEP_OFFSET = 15;
#else 
	const size_t BEGIN_CHARACTER_INDEX__ = 0xE100;
	const size_t END_CHARACTER_INDEX__ = 0xE176;
	const size_t END_SLEEP_OFFSET = 0;
#endif // WINDOWS10_STYLE
};

class SimpleInfinateLoadingWidget : public ElaText
{
	Q_OBJECT
public:
	SimpleInfinateLoadingWidget(QWidget* parent = nullptr);
	SimpleInfinateLoadingWidget(size_t FontSize, QWidget* parent = nullptr);
	~SimpleInfinateLoadingWidget();
	void start(bool start);
private:
	void RefreshLoadingAnimation();
	void ThemeChanged(ElaThemeType::ThemeMode theme);
	
private:
	ElaText* LoadingText__;
	QTimer* RefreshTimer__;
#ifdef USE_WINDOWS10_STYLE
	const size_t BEGIN_CHARACTER_INDEX__ = 0xE052;
	const size_t END_CHARACTER_INDEX__ = 0xE0CB;
	const size_t END_SLEEP_OFFSET = 15;
#else
	const size_t BEGIN_CHARACTER_INDEX__ = 0xE100;
	const size_t END_CHARACTER_INDEX__ = 0xE176;
	const size_t END_SLEEP_OFFSET = 0;
#endif // WINDOWS10_STYLE
	size_t LoadingAnimationCounter__;
};


class FluentProgressDialog : public ElaContentDialog
{
	Q_OBJECT
public:
	FluentProgressDialog(const QString& labelText, const QString& cancelButtonText, int minimum, int maximum, QWidget* parent);
	~FluentProgressDialog();
	void UpdatePercentage(int p);
	void UpdateInfo(QString info);
signals:
	void canceled();
private:
	FluentLoadingWidget* LoadingWidget;
};