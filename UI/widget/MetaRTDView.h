#pragma once
#include <QWidget>
#include <QKeyEvent>
#include <QPaintEvent>
#include <ElaTheme.h>
#include <QVector>

class MetaRTDView : public QWidget
{
	Q_OBJECT

public:
	enum class RTDViewType
	{
		SMALL_WINDOW,
		MIDDLE_WINDOW,
		LARGE_WINDOW,
		EXTEND_WINDOW
	};
public:
	MetaRTDView(RTDViewType type=RTDViewType::SMALL_WINDOW,QWidget *parent = nullptr);
	~MetaRTDView();

protected:
	void paintEvent(QPaintEvent* event);


private:
	void ThemeChangedSlot(ElaThemeType::ThemeMode mode);

private:
	RTDViewType ViewType__;
};
