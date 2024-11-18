#pragma once

#include <QWidget>
#include <QPixmap>
#include <QDialog>

class CustomSplashScreen  : public QDialog
{
	Q_OBJECT

public:
	CustomSplashScreen(size_t waitTime,QWidget* parent=nullptr);
	~CustomSplashScreen();

protected:
	void paintEvent(QPaintEvent* event) override;
private:
	void moveToCenter();
	const size_t FixedWindowWidth = 400;
	const size_t FixedWindowHeight = 248;
	const size_t shadowBorderWidth = 0;
	QPixmap* logo;
	QPixmap* OrgLogo;
	QTimer* CloseTimer;

	QTimer* RefreshTimer;
	size_t LoadingAnimationCounter__;
	unsigned short loadingIdx;

#define USE_WINDOWS10_STYLE
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
