#include "CustomSplashScreen.h"
#include "QPainter"
#include "QSvgRenderer"
#include "QRectF"

CustomSplashScreen::CustomSplashScreen(const QPixmap& pixmap , Qt::WindowFlags f )
	: QSplashScreen(pixmap,f)
{}

CustomSplashScreen::~CustomSplashScreen()
{}

void CustomSplashScreen::drawContents(QPainter * painter)
{
	QSvgRenderer render;
	render.load(QString(":/logo/Image/Splash_Screen.svg"));
	render.render(painter);
}
