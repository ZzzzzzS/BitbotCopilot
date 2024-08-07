#pragma once

#include "MetaRTDView.h"
#include "ui_RTDGamepadStatus.h"
#include "../../Utils/GamepadDriver/ZQGamepad.h"

QT_BEGIN_NAMESPACE
namespace Ui { class RTDGamepadStatusClass; };
QT_END_NAMESPACE

class RTDGamepadStatus : public MetaRTDView
{
	Q_OBJECT

public:
	RTDGamepadStatus(int ConnectedDevice,QWidget *parent = nullptr);
	~RTDGamepadStatus();

public: //slots
	void DeviceConnectionChanged(int id, bool connected);
	void JoystickMoved(int id, Q_XSX_JOYSTICK_ENUM axis, float value);

private:
	Ui::RTDGamepadStatusClass *ui;
	int connectedDeviceCount = 0;
};
