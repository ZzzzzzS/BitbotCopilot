#pragma once

#include <QSplashScreen>

class CustomSplashScreen  : public QSplashScreen
{
	Q_OBJECT

public:
	CustomSplashScreen(const QPixmap& pixmap = QPixmap(), Qt::WindowFlags f = Qt::WindowFlags());
	~CustomSplashScreen();
private:
	void drawContents(QPainter* painter);
};
