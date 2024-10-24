#include "SimpleTrackpad.h"
#include "QDebug"
#include "QTouchEvent"
#include "QVBoxLayout"
#include "QHBoxLayout"
#include "QSpacerItem"
#include "QGraphicsDropShadowEffect"
#include "QPainter"
#include "QStyle"
#include "QStyleOption"
#include "ElaApplication.h"

SimpleTrackpad::SimpleTrackpad(QWidget* parent)
	:QWidget(parent),
	CallMinInterval__(100)
{
	this->setAttribute(Qt::WA_AcceptTouchEvents);
	//this->setAttribute(Qt::WA_TranslucentBackground);
	this->setObjectName("Background");
	this->setAutoFillBackground(false);

	this->PadName__ = new ElaText(this);
	this->XAxisName__ = new ElaText("X: 0.0", this);
	this->YAxisName__ = new ElaText("Y: 0.0", this);
	QFont font = this->PadName__->font();
	font.setPointSize(10);
	this->PadName__->setFont(font);
	this->XAxisName__->setFont(font);
	this->YAxisName__->setFont(font);
	this->PadName__->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);
	this->XAxisName__->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
	this->YAxisName__->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

	QHBoxLayout* HLay = new QHBoxLayout();
	HLay->addWidget(this->PadName__);
	HLay->addWidget(this->XAxisName__);
	HLay->addWidget(this->YAxisName__);

	QVBoxLayout* VLay = new QVBoxLayout(this);
	auto spacer=new QSpacerItem(20, 40, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);
	VLay->addLayout(HLay);
	VLay->addSpacerItem(spacer);

	this->isEnableMica__ = eApp->getIsEnableMica();
	connect(eApp, &ElaApplication::pIsEnableMicaChanged, this, [=]() {
		this->isEnableMica__ = eApp->getIsEnableMica();
		update();
		});
	eApp->syncMica(this);
	this->ThemeChanged(eTheme->getThemeMode());
	QObject::connect(eTheme, &ElaTheme::themeModeChanged, this, &SimpleTrackpad::ThemeChanged);
}

SimpleTrackpad::~SimpleTrackpad()
{
}

void SimpleTrackpad::SetName(QString name)
{
	this->PadName__->setText(name);
}

void SimpleTrackpad::paintEvent(QPaintEvent* event)
{
	QStyleOption opt;
	opt.initFrom(this);
	QPainter p(this);
	style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);

	
	QPainter painter(this);
	
	if (!this->isEnableMica__)
	{
		painter.save();
		painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
		painter.setPen(Qt::NoPen);
		painter.setBrush(ElaThemeColor(eTheme->getThemeMode(), WindowBase));
		painter.drawRect(rect());
		painter.restore();
	}
	
	painter.setRenderHints(QPainter::SmoothPixmapTransform | QPainter::Antialiasing | QPainter::TextAntialiasing);  // 反锯齿;
	
	painter.save();
	painter.setBrush(QBrush(this->WindowColor__));
	painter.setPen(QPen(QBrush(this->WindowEdgeColor__), 3, Qt::SolidLine, Qt::FlatCap, Qt::MiterJoin));
	painter.drawRoundedRect(this->rect(), 10, 10);
	painter.restore();

	painter.save();
	painter.setPen(QPen(QBrush(this->TrajectoryColor__), 25,Qt::SolidLine,Qt::RoundCap,Qt::RoundJoin));
	if (this->TouchTrajectory__.size() > 10)
	{
		PointList p(this->TouchTrajectory__.end() - 10, this->TouchTrajectory__.end());
		painter.drawPolyline(p);
	}
	else
	{
		painter.drawPolyline(this->TouchTrajectory__);
	}
	painter.restore();

	QWidget::paintEvent(event);
}

bool SimpleTrackpad::event(QEvent* event)
{
	QTouchEvent* touchevent;
	if (event->type() == QEvent::TouchBegin)
	{
		touchevent = dynamic_cast<QTouchEvent*>(event);
		if (touchevent == nullptr)
		{
			qDebug() << "touch event cast failed!";
			return false;
		}
		event->accept();
		this->touchBeginEvent(touchevent);
		return true;
	}
	else if (event->type() == QEvent::TouchEnd)
	{
		touchevent = dynamic_cast<QTouchEvent*>(event);
		if (touchevent == nullptr)
		{
			qDebug() << "touch event cast failed!";
			return false;
		}
		event->accept();
		this->touchEndEvent(touchevent);
		return true;
	}
	else if (event->type() == QEvent::TouchUpdate)
	{
		touchevent = dynamic_cast<QTouchEvent*>(event);
		if (touchevent == nullptr)
		{
			qDebug() << "touch event cast failed!";
			return false;
		}
		event->accept();
		this->touchUpdateEvent(touchevent);
		return true;
	}
	else if (event->type() == QEvent::TouchCancel)
	{
		touchevent = dynamic_cast<QTouchEvent*>(event);
		if (touchevent == nullptr)
		{
			qDebug() << "touch event cast failed!";
			return false;
		}
		event->accept();
		this->touchCancelEvent(touchevent);
		return true;
	}
	return QWidget::event(event);
}

void SimpleTrackpad::touchBeginEvent(QTouchEvent* event)
{
	this->TouchTrajectory__.clear();
	this->TouchTrajectory__.reserve(5000);
	this->update();
	if (event->pointCount() != 1)
		return;

	QEventPoint p = event->point(0);
	this->StartPoint__ = p.position().toPoint();
	this->TouchTrajectory__.push_back(this->StartPoint__);
	QPointF ScaledPos = this->ComputeRelativeCoordinate(p.position());
	this->XAxisName__->setText(QString("X: ") + QString::number(ScaledPos.x(), 'g', 3));
	this->YAxisName__->setText(QString("Y: ") + QString::number(ScaledPos.y(), 'g', 3));
	emit this->TouchPositionChanged(ScaledPos);
}

void SimpleTrackpad::touchEndEvent(QTouchEvent* event)
{
	this->TouchTrajectory__.clear();
	this->XAxisName__->setText(QString("X: 0.0"));
	this->YAxisName__->setText(QString("Y: 0.0"));
	emit this->TouchPositionChanged(QPointF(0.0, 0.0));
	this->update();
}

void SimpleTrackpad::touchUpdateEvent(QTouchEvent* event)
{
	if (event->pointCount() != 1)
		return;
	QEventPoint p = event->point(0);
    this->TouchTrajectory__.push_back(p.position().toPoint());
	QPointF ScaledPos = this->ComputeRelativeCoordinate(p.position());
	this->XAxisName__->setText(QString("X: ") + QString::number(ScaledPos.x(), 'g', 3));
	this->YAxisName__->setText(QString("Y: ") + QString::number(ScaledPos.y(), 'g', 3));
	this->update();
	auto now = std::chrono::system_clock::now();
	auto duration = now - this->LastTrackpadCall__;
	long long ms = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
	if (ms < this->CallMinInterval__)
	{
		return; //block call
	}
	this->LastTrackpadCall__ = now;
	emit this->TouchPositionChanged(ScaledPos);
}

void SimpleTrackpad::touchCancelEvent(QTouchEvent* event)
{
	this->TouchPositionChanged(QPointF(0.0, 0.0));
	this->XAxisName__->setText(QString("X: 0.0"));
	this->YAxisName__->setText(QString("Y: 0.0"));
	this->TouchTrajectory__.clear();
	this->update();
}

QPointF SimpleTrackpad::ComputeRelativeCoordinate(QPointF wpos)
{
	QPointF StartPoint = this->StartPoint__.toPointF();
	QPointF SPos;
	SPos.setX(StartPoint.x() - wpos.x());
	SPos.setY(StartPoint.y() - wpos.y());
	//qDebug() << wpos << StartPoint;

	SPos.rx() /= (this->width() / 2);
	SPos.ry() /= (this->height() / 2);
	
	SPos.setX(std::clamp(SPos.x(), -1.0, 1.0));
	SPos.setY(std::clamp(SPos.y(), -1.0, 1.0));
	return SPos;
}

void SimpleTrackpad::ThemeChanged(ElaThemeType::ThemeMode mode)
{
	QPalette LabelPalette;
	QPalette MainLabelPalette;
	if (mode == ElaThemeType::ThemeMode::Dark)
	{
		QBrush brush_subLabel(QColor(206, 206, 206, 255));
		brush_subLabel.setStyle(Qt::SolidPattern);
		LabelPalette.setBrush(QPalette::Active, QPalette::WindowText, brush_subLabel);
		LabelPalette.setBrush(QPalette::Inactive, QPalette::WindowText, brush_subLabel);

		QBrush brush_mainLabel(QColor(255, 255, 255, 255));
		brush_mainLabel.setStyle(Qt::SolidPattern);
		MainLabelPalette.setBrush(QPalette::Active, QPalette::WindowText, brush_mainLabel);
		MainLabelPalette.setBrush(QPalette::Inactive, QPalette::WindowText, brush_mainLabel);

		this->WindowColor__ = QColor(60, 60, 60, 128);
		this->WindowEdgeColor__ = QColor(100, 100, 100, 200);
		//this->TrajectoryColor__ = QColor(16, 96, 220, 128);
		this->TrajectoryColor__ = QColor(200,200,200, 128);
	}
	else
	{
		QBrush brush_subLabel(QColor(55, 55, 55, 255));
		brush_subLabel.setStyle(Qt::SolidPattern);
		LabelPalette.setBrush(QPalette::Active, QPalette::WindowText, brush_subLabel);
		LabelPalette.setBrush(QPalette::Inactive, QPalette::WindowText, brush_subLabel);

		QBrush brush_mainLabel(QColor(0, 0, 0, 255));
		brush_mainLabel.setStyle(Qt::SolidPattern);
		MainLabelPalette.setBrush(QPalette::Active, QPalette::WindowText, brush_mainLabel);
		MainLabelPalette.setBrush(QPalette::Inactive, QPalette::WindowText, brush_mainLabel);

		this->WindowColor__ = QColor(250, 250, 250, 200);
		this->WindowEdgeColor__ = QColor(200, 200, 200, 128);
		//this->TrajectoryColor__ = QColor(55, 122, 231, 128);
		this->TrajectoryColor__ = QColor(220,220,220, 200);
	}
	this->PadName__->setPalette(MainLabelPalette);
	this->XAxisName__->setPalette(LabelPalette);
	this->YAxisName__->setPalette(LabelPalette);
	this->update();
}
