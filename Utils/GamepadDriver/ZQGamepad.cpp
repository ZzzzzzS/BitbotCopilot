#include "ZQGamepad.h"
#include <QMetaType>
#ifdef Q_OS_WIN
#include "ZGamepad_XInput.hpp"
#endif // QT_OS_WIN

ZQGamepad::ZQGamepad(uint cps,QObject* parent)
	:QObject(parent)
	
{
	qRegisterMetaType<Q_XSX_JOYSTICK_ENUM>("Q_XSX_JOYSTICK_ENUM");
#ifdef Q_OS_WIN
	this->GamepadBackendXInput = new zzs::ZGAMEPAD_XINPUT(cps);
	this->GamepadBackendXInput->RegistCallback(
		[this](std::vector<std::tuple<UINT, INT, BOOL>> ButtonStates) {
			for (auto state : ButtonStates)
			{
				std::thread::id id = std::this_thread::get_id();
				std::cout << "Sender ID: " << id << std::endl;
				emit this->JoystickMoved(std::get<0>(state), static_cast<Q_XSX_JOYSTICK_ENUM>(std::get<1>(state)), std::get<2>(state));
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

bool ZQGamepad::SetRumble(int id, quint16 LeftMotor, quint16 RightMotor, quint32 Duration)
{
#ifdef Q_OS_WIN
	return this->GamepadBackendXInput->setRumble(id, LeftMotor, RightMotor, Duration);
#endif // QT_OS_WIN
}
