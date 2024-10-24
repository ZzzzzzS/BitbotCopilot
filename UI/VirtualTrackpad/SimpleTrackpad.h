#pragma once
#include "QWidget"
#include "ElaTheme.h"
#include "QPaintEvent"
#include "QPolygon"
#include "QList"
#include "QPointF"
#include "ElaText.h"
#include "chrono"

class SimpleTrackpad : public QWidget
{
	Q_OBJECT
public:
	SimpleTrackpad(QWidget* parent = nullptr);
	~SimpleTrackpad();
	void SetName(QString name);

signals:
	void TouchPositionChanged(QPointF);
protected:
	void paintEvent(QPaintEvent* event);
	bool event(QEvent* event);
private:
	void touchBeginEvent(QTouchEvent* event);
	void touchEndEvent(QTouchEvent* event);
	void touchUpdateEvent(QTouchEvent* event);
	void touchCancelEvent(QTouchEvent* event);

	QPointF ComputeRelativeCoordinate(QPointF wpos);
	
	void ThemeChanged(ElaThemeType::ThemeMode mode);
private:
	using PointList = QList<QPoint>;
	PointList TouchTrajectory__;
	QPoint StartPoint__;

	ElaText* PadName__;
	ElaText* XAxisName__;
	ElaText* YAxisName__;
	
	QColor WindowColor__;
	QColor WindowEdgeColor__;
	QColor TrajectoryColor__;

	std::chrono::system_clock::time_point LastTrackpadCall__;
	const long long CallMinInterval__;

	bool isEnableMica__;
};