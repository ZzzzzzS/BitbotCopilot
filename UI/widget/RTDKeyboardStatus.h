#pragma once

#include "MetaRTDView.h"
#include "ui_RTDKeyboardStatus.h"
#include "ElaTheme.h"
#include <QVBoxLayout>
#include "ElaScrollPageArea.h"

QT_BEGIN_NAMESPACE
namespace Ui { class RTDKeyboardStatusClass; };
QT_END_NAMESPACE

class RTDKeyboardStatus : public MetaRTDView
{
	Q_OBJECT

public:
	RTDKeyboardStatus(QWidget *parent = nullptr);
	~RTDKeyboardStatus();
	void ResetUI();
public: //slot
	void ButtonClicked(QString Button, bool Press);

private:
	void ScrollAreaThemeChangedSlot(ElaThemeType::ThemeMode mode);
	ElaScrollPageArea* AppendAction(const QString& Button, bool Press);

private:
	Ui::RTDKeyboardStatusClass *ui;
	QVector<ElaScrollPageArea*> ActionHistoryWidgets;
	QVBoxLayout* CentralLayout;
	QWidget* CentralWidget;
};
