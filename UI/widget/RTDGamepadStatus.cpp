#include "RTDGamepadStatus.h"
#include <QPalette>

RTDGamepadStatus::RTDGamepadStatus(int ConnectedDevice,QWidget *parent)
	: MetaRTDView(MetaRTDView::RTDViewType::SMALL_WINDOW,parent)
	, ui(new Ui::RTDGamepadStatusClass()),
	connectedDeviceCount(ConnectedDevice)
{
	ui->setupUi(this);
	this->ui->label_ConnectedDevice->setText(QString::number(connectedDeviceCount));
	if (eTheme->getThemeMode() == ElaThemeType::Light)
	{
		this->ui->label_gamepad->setPixmap(QPixmap(":/UI/Image/gamepad_icon.png").scaledToHeight(65));
		QPalette pe = this->ui->label_status->palette();
		pe.setColor(QPalette::WindowText, Qt::black);
		if (this->connectedDeviceCount == 1)
		{
			pe.setColor(QPalette::WindowText, Qt::darkGreen);
		}
		else if (this->connectedDeviceCount > 1)
		{
			pe.setColor(QPalette::WindowText, Qt::red);
		}
		this->ui->label_ConnectedDevice->setPalette(pe);
		this->ui->label_dev_cnt->setPalette(pe);
	}
	else
	{
		this->ui->label_gamepad->setPixmap(QPixmap(":/UI/Image/gamepad_dark.png").scaledToHeight(65));
		QPalette pe = this->ui->label_status->palette();
		pe.setColor(QPalette::WindowText, Qt::white);
		if (this->connectedDeviceCount == 1)
		{
			pe.setColor(QPalette::WindowText, Qt::green);
		}
		else if (this->connectedDeviceCount > 1)
		{
			pe.setColor(QPalette::WindowText, Qt::red);
		}
		this->ui->label_ConnectedDevice->setPalette(pe);
		this->ui->label_dev_cnt->setPalette(pe);
	}
	QObject::connect(eTheme, &ElaTheme::themeModeChanged, this, [this](ElaThemeType::ThemeMode mod) {
		if (mod == ElaThemeType::Light)
		{
			this->ui->label_gamepad->setPixmap(QPixmap(":/UI/Image/gamepad_icon.png").scaledToHeight(65));
			QPalette pe = this->ui->label_status->palette();
			pe.setColor(QPalette::WindowText, Qt::black);
			if (this->connectedDeviceCount == 1)
			{
				pe.setColor(QPalette::WindowText, Qt::darkGreen);
			}
			else if (this->connectedDeviceCount > 1)
			{
				pe.setColor(QPalette::WindowText, Qt::red);
			}
			this->ui->label_ConnectedDevice->setPalette(pe);
			this->ui->label_dev_cnt->setPalette(pe);
		}
		else
		{
			this->ui->label_gamepad->setPixmap(QPixmap(":/UI/Image/gamepad_dark.png").scaledToHeight(65));
			QPalette pe = this->ui->label_status->palette();
			pe.setColor(QPalette::WindowText, Qt::white);
			if (this->connectedDeviceCount == 1)
			{
				pe.setColor(QPalette::WindowText, Qt::green);
			}
			else if (this->connectedDeviceCount > 1)
			{
				pe.setColor(QPalette::WindowText, Qt::red);
			}
			this->ui->label_ConnectedDevice->setPalette(pe);
			this->ui->label_dev_cnt->setPalette(pe);
		}
		});
}

RTDGamepadStatus::~RTDGamepadStatus()
{
	delete ui;
}

void RTDGamepadStatus::DeviceConnectionChanged(int id, bool connected)
{
	if (connected)
		this->connectedDeviceCount++;
	else
		this->connectedDeviceCount--;
	
	QPalette pe = this->ui->label_status->palette();
	if (this->connectedDeviceCount == 1)
	{
		if (eTheme->getThemeMode() == ElaThemeType::Light)
		{
			pe.setColor(QPalette::WindowText, Qt::darkGreen);
		}
		else
		{
			pe.setColor(QPalette::WindowText, Qt::green);
		}
	}
	else if (this->connectedDeviceCount > 1)
	{
		pe.setColor(QPalette::WindowText, Qt::red);
	}
	this->ui->label_ConnectedDevice->setPalette(pe);
	this->ui->label_dev_cnt->setPalette(pe);
	this->ui->label_ConnectedDevice->setText(QString::number(connectedDeviceCount));
}

void RTDGamepadStatus::JoystickMoved(int id, Q_XSX_JOYSTICK_ENUM axis, float value)
{
	QString v = QString::number(value);
	switch (axis)
	{
	case Q_XSX_JOYSTICK_ENUM::JoystickLeftX:
		this->ui->label_LX->setText(v);
		break;
	case Q_XSX_JOYSTICK_ENUM::JoystickLeftY:
		this->ui->Label_LY->setText(v);
		break;
	case Q_XSX_JOYSTICK_ENUM::JoystickRightX:
		this->ui->label_RX->setText(v);
		break;
	case Q_XSX_JOYSTICK_ENUM::JoystickRightY:
		this->ui->label_RY->setText(v);
		break;
	case Q_XSX_JOYSTICK_ENUM::TriggerLeft:
		this->ui->Label_LT->setText(v);
		break;
	case Q_XSX_JOYSTICK_ENUM::TriggerRight:
		this->ui->label_RT->setText(v);
		break;
	default:
		break;
	}
}
