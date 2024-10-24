#include "VirtualTrackpad.h"
#include "./ui_VirtualTrackpad.h"
#include "ElaTheme.h"
#include "ElaPushButton.h"

VirtualTrackpad::VirtualTrackpad(QWidget* parent)
	:QWidget(parent),
	ui(new Ui::VirtualTrackpad)
{
	this->ui->setupUi(this);
	this->setFixedHeight(300);
	this->ui->widget_connected->hide();
	this->ui->widget_disconnect->show();

	this->ui->scrollArea->setIsGrabGesture(true, 0);
	this->ui->scrollArea->setWidgetResizable(true);
	this->ButtonsUI__ = new QWidget(this);
	this->ButtonsUI__->setStyleSheet(R"(
		background-color:rgba(0,0,0,0);
		border:0px solid rgba(200, 200, 200, 0);
		)");
	this->ui->scrollArea->setStyleSheet(R"(
		background-color:rgba(0,0,0,0);
		border:0px solid rgba(200, 200, 200, 0);
		)");

	this->ui->scrollArea->setWidget(this->ButtonsUI__);
	this->ButtonsLayout__ = new ElaFlowLayout(this->ButtonsUI__);
	this->ButtonsLayout__->setIsAnimation(true);

	this->ui->widget_LeftTrack->SetName(tr("Left Trackpad "));
	this->ui->widget_RightTrack->SetName(tr("Right Trackpad "));

	QObject::connect(this->ui->widget_LeftTrack, &SimpleTrackpad::TouchPositionChanged, this, [this](QPointF p) {
		emit this->VirtualTrackpadMoved("GAMEPAD_LX", p.x(), "GAMEPAD_LY", p.y());
		qDebug() << "L" << p;
	});

	QObject::connect(this->ui->widget_RightTrack, &SimpleTrackpad::TouchPositionChanged, this, [this](QPointF p) {
		emit this->VirtualTrackpadMoved("GAMEPAD_RX", p.x(), "GAMEPAD_RY", p.y());
		qDebug() << "R" << p;
	});

	//this->ui->widget_disconnect->hide();
	//this->ui->widget_connected->show();

	this->SetTheme(eTheme->getThemeMode());
	QObject::connect(eTheme, &ElaTheme::themeModeChanged, this, &VirtualTrackpad::SetTheme);

	//QMap<QString, QString> testkeys;
	//testkeys.insert("1", "event1");
	//testkeys.insert("2", "event2");
	//testkeys.insert("3", "event3");
	//testkeys.insert("4", "event4");
	//testkeys.insert("5", "event4");
	//this->UpdateAvailableButton(testkeys);
}

VirtualTrackpad::~VirtualTrackpad()
{
}

void VirtualTrackpad::setConnected(bool connectionStatus)
{
	if (connectionStatus == true)
	{
		this->ui->widget_disconnect->hide();
		this->ui->widget_connected->show();
	}
	else
	{
		for (auto [key, value] : this->ButtonList__.asKeyValueRange())
		{
			this->ButtonsLayout__->removeWidget(key);
			delete key;
		}
		this->ButtonList__.clear();

		this->ui->widget_connected->hide();
		this->ui->widget_disconnect->show();
	}
}

void VirtualTrackpad::UpdateAvailableButton(const QMap<QString, QString>& buttons) //key event
{
	QMap<QString, QList<QString>> event_keylist;
	for (auto [key, value] : buttons.asKeyValueRange())
	{
		if (key.contains("GAMEPAD_L") || key.contains("GAMEPAD_R"))
			continue;

		if (event_keylist.contains(value))
		{
			event_keylist[value].push_back(key);
		}
		else
		{
			QList<QString> list;
			list.push_back(key);
			event_keylist.insert(value, list);
		}
	}

	if (!this->ButtonList__.isEmpty())
	{
		for (auto [key, value] : this->ButtonList__.asKeyValueRange())
		{
			this->ButtonsLayout__->removeWidget(key);
			delete key;
		}
		this->ButtonList__.clear();
	}

	for (auto [key, value] : event_keylist.asKeyValueRange())
	{
		auto wid = new ElaPushButton(this);
		wid->setFixedSize(165, 80);

		QString Text = key;
		for (auto&& b : value)
		{
			if (b != " ")
				Text += "\n(" + b + ")";
			else
				Text += "\n(Space)";
		}
		wid->setText(Text);
		QFont f = wid->font();
		f.setPointSize(10);
		wid->setFont(f);
		this->ButtonsLayout__->addWidget(wid);
		this->ButtonList__.insert(wid, key);
		
		QObject::connect(wid, &ElaPushButton::pressed, this, [this, key]() {
			emit this->VirtualButtonPressed(key,1);
			qDebug() << key << " pressed";
			});
		QObject::connect(wid, &ElaPushButton::released, this, [this, key]() {
			emit this->VirtualButtonPressed(key, 2);
			qDebug() << key << " released";
			});
	}
}

void VirtualTrackpad::SetTheme(ElaThemeType::ThemeMode mode)
{
	QPalette LabelPalette;
	QPalette MainLabelPalette;
	if (mode == ElaThemeType::ThemeMode::Dark)
	{
		QBrush brush_subLabel(QColor(206, 206, 206, 255));
		brush_subLabel.setStyle(Qt::SolidPattern);
		LabelPalette.setBrush(QPalette::Active, QPalette::WindowText, brush_subLabel);
		LabelPalette.setBrush(QPalette::Inactive, QPalette::WindowText, brush_subLabel);

		QBrush brush_mainLabel(QColor(255,255,255, 255));
		brush_mainLabel.setStyle(Qt::SolidPattern);
		MainLabelPalette.setBrush(QPalette::Active, QPalette::WindowText, brush_mainLabel);
		MainLabelPalette.setBrush(QPalette::Inactive, QPalette::WindowText, brush_mainLabel);
	}
	else
	{
		QBrush brush_subLabel(QColor(55, 55, 55, 255));
		brush_subLabel.setStyle(Qt::SolidPattern);
		LabelPalette.setBrush(QPalette::Active, QPalette::WindowText, brush_subLabel);
		LabelPalette.setBrush(QPalette::Inactive, QPalette::WindowText, brush_subLabel);

		QBrush brush_mainLabel(QColor(0,0,0, 255));
		brush_mainLabel.setStyle(Qt::SolidPattern);
		MainLabelPalette.setBrush(QPalette::Active, QPalette::WindowText, brush_mainLabel);
		MainLabelPalette.setBrush(QPalette::Inactive, QPalette::WindowText, brush_mainLabel);
	}
	this->ui->label_subtitle->setPalette(LabelPalette);
	this->ui->label_2->setPalette(MainLabelPalette);
	this->ui->label_buttons->setPalette(MainLabelPalette);
}
