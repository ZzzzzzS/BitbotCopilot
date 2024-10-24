#pragma once
#include <QWidget>
#include "ElaTheme.h"
#include "ElaFlowLayout.h"
#include "ElaText.h"
#include "ElaPushButton.h"
#include "QMap"
#include "QString"


QT_BEGIN_NAMESPACE
namespace Ui {
	class VirtualTrackpad;
}
QT_END_NAMESPACE

class VirtualTrackpad : public QWidget
{
	Q_OBJECT
public:
	VirtualTrackpad(QWidget* parent=nullptr);
	~VirtualTrackpad();

	void setConnected(bool connectionStatus);
	void UpdateAvailableButton(const QMap<QString, QString>& buttons); //key, event

signals:
	void VirtualButtonPressed(QString, int);
	void VirtualTrackpadMoved(QString, double, QString, double);

private:
	void SetTheme(ElaThemeType::ThemeMode mode);
private:
	Ui::VirtualTrackpad* ui;
	QWidget* ButtonsUI__;
	ElaFlowLayout* ButtonsLayout__;
	QMap<ElaPushButton*, QString> ButtonList__;
};