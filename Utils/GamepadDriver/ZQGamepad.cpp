#include "ZQGamepad.h"
#include <QMetaType>
#ifdef Q_OS_WIN
#include "ZGamepad_XInput.hpp"
#endif // QT_OS_WIN
#include "QDebug"

ZQGamepad::ZQGamepad(uint cps, QObject* parent)
	:QObject(parent)

{
	this->InitButtonNameMap();
	qRegisterMetaType<Q_XSX_JOYSTICK_ENUM>("Q_XSX_JOYSTICK_ENUM");
#ifdef Q_OS_WIN
	this->GamepadBackendXInput = new zzs::ZGAMEPAD_XINPUT(cps);
	this->GamepadBackendXInput->RegistCallback(
		[this](std::vector<std::tuple<UINT, INT, BOOL>> ButtonStates) {
			for (auto state : ButtonStates)
			{
				std::thread::id id = std::this_thread::get_id();
				qDebug() << "button changed" << std::get<0>(state) << " " << std::get<1>(state) << " " << std::get<2>(state);
				emit this->ButtonClicked(std::get<0>(state), static_cast<Q_XSX_JOYSTICK_ENUM>(std::get<1>(state)), std::get<2>(state));
			}
		},
		[this](std::vector<std::tuple<UINT, INT, FLOAT>> JoystickStates) {
			for (auto state : JoystickStates)
			{
				emit this->JoystickMoved(std::get<0>(state), static_cast<Q_XSX_JOYSTICK_ENUM>(std::get<1>(state)), std::get<2>(state));
			}
		},
		[this](DWORD id, BOOL states) {
			emit this->ConnectionChanged(id, states);
		}
	);

#endif // QT_OS_WIN
}

ZQGamepad::~ZQGamepad()
{
#ifdef Q_OS_WIN
	delete this->GamepadBackendXInput;
#endif // QT_OS_WIN
}

void ZQGamepad::start()
{
#ifdef Q_OS_WIN
	this->GamepadBackendXInput->start();
#endif // QT_OS_WIN
}

size_t ZQGamepad::CountConnectedGamepad()
{
#ifdef Q_OS_WIN
	auto set = this->GamepadBackendXInput->getAvailableGamepadID();
	return set.size();
#endif
	return 0;
}

QSet<size_t> ZQGamepad::getAvailableGamepadID()
{
#ifdef Q_OS_WIN
	auto stdset = this->GamepadBackendXInput->getAvailableGamepadID();
	QSet<size_t> Gamepads;
	for (auto& i : stdset)
	{
		Gamepads.insert(static_cast<size_t>(i));
	}
	return Gamepads;
#endif
	return QSet<size_t>();
}

bool ZQGamepad::SetRumble(int id, quint16 LeftMotor, quint16 RightMotor, quint32 Duration)
{
#ifdef Q_OS_WIN
	return this->GamepadBackendXInput->setRumble(id, LeftMotor, RightMotor, Duration);
#endif // QT_OS_WIN
	return false;
}


void ZQGamepad::InitButtonNameMap()
{
	//TODO: set the button name with respected to json settings.
	this->GamepadButtonNameMap.insert(Q_XSX_JOYSTICK_ENUM::Button_A, QString("GAMEPAD_BUTTON_A"));
	this->GamepadButtonNameMap.insert(Q_XSX_JOYSTICK_ENUM::Button_B, QString("GAMEPAD_BUTTON_B"));
	this->GamepadButtonNameMap.insert(Q_XSX_JOYSTICK_ENUM::Button_X, QString("GAMEPAD_BUTTON_X"));
	this->GamepadButtonNameMap.insert(Q_XSX_JOYSTICK_ENUM::Button_Y, QString("GAMEPAD_BUTTON_Y"));

	this->GamepadButtonNameMap.insert(Q_XSX_JOYSTICK_ENUM::ButtonLeftBumper, QString("GAMEPAD_BUTTON_LB"));
	this->GamepadButtonNameMap.insert(Q_XSX_JOYSTICK_ENUM::ButtonRightBumper, QString("GAMEPAD_BUTTON_RB"));

	this->GamepadButtonNameMap.insert(Q_XSX_JOYSTICK_ENUM::ButtonSwitch, QString("GAMEPAD_BUTTON_SWITCH"));
	this->GamepadButtonNameMap.insert(Q_XSX_JOYSTICK_ENUM::ButtonMenu, QString("GAMEPAD_BUTTON_MENU"));

	this->GamepadButtonNameMap.insert(Q_XSX_JOYSTICK_ENUM::ButtonLeftStick, QString("GAMEPAD_BUTTON_L_STICK"));
	this->GamepadButtonNameMap.insert(Q_XSX_JOYSTICK_ENUM::ButtonRightStick, QString("GAMEPAD_BUTTON_R_STICK"));

	this->GamepadButtonNameMap.insert(Q_XSX_JOYSTICK_ENUM::ButtonPadUp, QString("GAMEPAD_HAT_UP"));
	this->GamepadButtonNameMap.insert(Q_XSX_JOYSTICK_ENUM::ButtonPadDown, QString("GAMEPAD_HAT_DOWN"));
	this->GamepadButtonNameMap.insert(Q_XSX_JOYSTICK_ENUM::ButtonPadLeft, QString("GAMEPAD_HAT_LEFT"));
	this->GamepadButtonNameMap.insert(Q_XSX_JOYSTICK_ENUM::ButtonPadRight, QString("GAMEPAD_HAT_RIGHT"));


	this->GamepadButtonNameMap.insert(Q_XSX_JOYSTICK_ENUM::JoystickLeftX, QString("GAMEPAD_LX"));
	this->GamepadButtonNameMap.insert(Q_XSX_JOYSTICK_ENUM::JoystickLeftY, QString("GAMEPAD_LY"));
	this->GamepadButtonNameMap.insert(Q_XSX_JOYSTICK_ENUM::JoystickRightX, QString("GAMEPAD_RX"));
	this->GamepadButtonNameMap.insert(Q_XSX_JOYSTICK_ENUM::JoystickRightY, QString("GAMEPAD_RY"));
	this->GamepadButtonNameMap.insert(Q_XSX_JOYSTICK_ENUM::TriggerLeft, QString("GAMEPAD_LT"));
	this->GamepadButtonNameMap.insert(Q_XSX_JOYSTICK_ENUM::TriggerRight, QString("GAMEPAD_RT"));
}

QString ZQGamepad::name(const Q_XSX_JOYSTICK_ENUM& key)
{
	if (this->GamepadButtonNameMap.contains(key))
	{
		return this->GamepadButtonNameMap[key];
	}
	else
	{
		return QString("GAMEPAD_UNDEFINED");
	}
}