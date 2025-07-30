#include "ZQGamepad.h"
#include <QMetaType>
#ifdef USE_DIRECT_XINPUT
#include "ZGamepad_XInput.hpp"
#endif // QT_OS_WIN
#include "QDebug"
#include "QGamepad"
#include "QTimer"

ZQGamepad::ZQGamepad(uint cps, QObject* parent)
	:QObject(parent)

{
	this->InitButtonNameMap();
	qRegisterMetaType<Q_XSX_JOYSTICK_ENUM>("Q_XSX_JOYSTICK_ENUM");
	qRegisterMetaType<QVector<Q_XSX_JOYSTICK_ENUM>>("QVector<Q_XSX_JOYSTICK_ENUM>");
	qRegisterMetaType<QVector<std::tuple<int, Q_XSX_JOYSTICK_ENUM, float>>>("QVector<std::tuple<int, Q_XSX_JOYSTICK_ENUM, float>>");
#ifdef USE_DIRECT_XINPUT
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

#else
	this->GamepadManager__ = QGamepadManager::instance();
	this->GamepadRefreshTimer__ = new QTimer(this);
	QObject::connect(this->GamepadRefreshTimer__, &QTimer::timeout, this, &ZQGamepad::RefreshJoystickStates);
	QObject::connect(this->GamepadManager__, &QGamepadManager::connectedGamepadsChanged, this, &ZQGamepad::RefreshConnectedGamepad);
	this->RefreshConnectedGamepad();
	this->GamepadRefreshTimer__->start(1000 / cps);
#endif // QT_OS_WIN

}

ZQGamepad::~ZQGamepad()
{
#ifdef USE_DIRECT_XINPUT
	delete this->GamepadBackendXInput;
#endif // QT_OS_WIN
}

void ZQGamepad::start()
{
#ifdef USE_DIRECT_XINPUT
	this->GamepadBackendXInput->start();
#endif // QT_OS_WIN
}

size_t ZQGamepad::CountConnectedGamepad()
{
#ifdef USE_DIRECT_XINPUT
	auto set = this->GamepadBackendXInput->getAvailableGamepadID();
	return set.size();
#else
	return this->GamepadMap__.size();
#endif

}

QSet<size_t> ZQGamepad::getAvailableGamepadID()
{
#ifdef USE_DIRECT_XINPUT
	auto stdset = this->GamepadBackendXInput->getAvailableGamepadID();
	QSet<size_t> Gamepads;
	for (auto& i : stdset)
	{
		Gamepads.insert(static_cast<size_t>(i));
	}
	return Gamepads;

#else
	auto ID_list = this->GamepadMap__.keys();
	QSet<size_t> Gamepads;
	for (auto& i : ID_list)
	{
		Gamepads.insert(static_cast<size_t>(i));
	}

	return Gamepads;
#endif
}

bool ZQGamepad::SetRumble(int id, quint16 LeftMotor, quint16 RightMotor, quint32 Duration)
{
#ifdef USE_DIRECT_XINPUT
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
	this->GamepadButtonNameMap.insert(Q_XSX_JOYSTICK_ENUM::ButtonXbox, QString("GAMEPAD_BUTTON_XBOX"));
	this->GamepadButtonNameMap.insert(Q_XSX_JOYSTICK_ENUM::ButtonShare, QString("GAMEPAD_BUTTON_SHARE"));

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

void ZQGamepad::RefreshJoystickStates()
{
	QVector<std::tuple<int, Q_XSX_JOYSTICK_ENUM, double>> states;
	for (auto i = this->GamepadAxisValueMap__.begin(); i != this->GamepadAxisValueMap__.end(); ++i)
	{
		int id = i.key();
		auto gamepad = this->GamepadMap__.value(id);
		if (gamepad)
		{
			std::array<double, 6> axisValues = {
				gamepad->axisLeftX(),
				gamepad->axisLeftY(),
				gamepad->axisRightX(),
				gamepad->axisRightY(),
				gamepad->buttonL2(),
				gamepad->buttonR2()
			};



			constexpr double threshold = 0.01; // Define a threshold for axis value change
			if (std::abs(i.value()[0] - axisValues[0]) > threshold)
			{
				//emit this->JoystickMoved(id, Q_XSX_JOYSTICK_ENUM::JoystickLeftX, axisValues[0]);
				states.emplace_back(id, Q_XSX_JOYSTICK_ENUM::JoystickLeftX, axisValues[0]);
			}
			if (std::abs(i.value()[1] - axisValues[1]) > threshold)
			{
				//emit this->JoystickMoved(id, Q_XSX_JOYSTICK_ENUM::JoystickLeftY, axisValues[1]);
				states.emplace_back(id, Q_XSX_JOYSTICK_ENUM::JoystickLeftY, axisValues[1]);
			}
			if (std::abs(i.value()[2] - axisValues[2]) > threshold)
			{
				//emit this->JoystickMoved(id, Q_XSX_JOYSTICK_ENUM::JoystickRightX, axisValues[2]);
				states.emplace_back(id, Q_XSX_JOYSTICK_ENUM::JoystickRightX, axisValues[2]);
			}
			if (std::abs(i.value()[3] - axisValues[3]) > threshold)
			{
				//emit this->JoystickMoved(id, Q_XSX_JOYSTICK_ENUM::JoystickRightY, axisValues[3]);
				states.emplace_back(id, Q_XSX_JOYSTICK_ENUM::JoystickRightY, axisValues[3]);
			}
			if (std::abs(i.value()[4] - axisValues[4]) > threshold)
			{
				//emit this->JoystickMoved(id, Q_XSX_JOYSTICK_ENUM::TriggerLeft, axisValues[4]);
				states.emplace_back(id, Q_XSX_JOYSTICK_ENUM::TriggerLeft, axisValues[4]);
			}
			if (std::abs(i.value()[5] - axisValues[5]) > threshold)
			{
				//emit this->JoystickMoved(id, Q_XSX_JOYSTICK_ENUM::TriggerRight, axisValues[5]);
				states.emplace_back(id, Q_XSX_JOYSTICK_ENUM::TriggerRight, axisValues[5]);
			}
			// Update the axis values in the map

			i.value() = axisValues;
		}
	}
	if(!states.empty())
		emit this->JoysticksMoved(states);
}

void ZQGamepad::RefreshConnectedGamepad()
{
	QList<int> connectedGamepads = this->GamepadManager__->connectedGamepads();
	qDebug() << "connected gamepads" << connectedGamepads.size();
	QList<int> CurrentGamepadList = this->GamepadMap__.keys();

	QList<int> NewGamepadList;
	QList<int> RemovedGamepadList;
	for (int id : connectedGamepads)
	{
		if (!CurrentGamepadList.contains(id))
		{
			NewGamepadList.append(id);
		}
	}

	for (int id : CurrentGamepadList)
	{
		if (!connectedGamepads.contains(id))
		{
			RemovedGamepadList.append(id);
		}
	}

	for (int id : NewGamepadList)
	{
		this->RegisterGamepad(id);
		emit this->ConnectionChanged(id, true);
	}

	for (int id : RemovedGamepadList)
	{
		this->GamepadMap__.remove(id);
		this->GamepadAxisValueMap__.remove(id);
		emit this->ConnectionChanged(id, false);
	}

}

void ZQGamepad::RegisterGamepad(int id)
{
	auto gamepad = new QGamepad(id, this);
	this->GamepadMap__.insert(id, gamepad);

	std::array<double, 6> axisValues = {
		gamepad->axisLeftX(),
		gamepad->axisLeftY(),
		gamepad->axisRightX(),
		gamepad->axisRightY(),
		gamepad->buttonL2(),
		gamepad->buttonR2()
	};
	this->GamepadAxisValueMap__.insert(id, axisValues);


	QObject::connect(gamepad, &QGamepad::buttonAChanged, this, [this, id](bool value) {
		emit this->ButtonClicked(id, Q_XSX_JOYSTICK_ENUM::Button_A, this->GamepadMap__[id]->buttonA());
		});
	QObject::connect(gamepad, &QGamepad::buttonBChanged, this, [this, id](bool value) {
		emit this->ButtonClicked(id, Q_XSX_JOYSTICK_ENUM::Button_B, this->GamepadMap__[id]->buttonB());
		});
	QObject::connect(gamepad, &QGamepad::buttonXChanged, this, [this, id](bool value) {
		emit this->ButtonClicked(id, Q_XSX_JOYSTICK_ENUM::Button_X, this->GamepadMap__[id]->buttonX());
		});
	QObject::connect(gamepad, &QGamepad::buttonYChanged, this, [this, id](bool value) {
		emit this->ButtonClicked(id, Q_XSX_JOYSTICK_ENUM::Button_Y, this->GamepadMap__[id]->buttonY());
		});

	QObject::connect(gamepad, &QGamepad::buttonL1Changed, this, [this, id](bool value) {
		emit this->ButtonClicked(id, Q_XSX_JOYSTICK_ENUM::ButtonLeftBumper, this->GamepadMap__[id]->buttonL1());
		});
	QObject::connect(gamepad, &QGamepad::buttonR1Changed, this, [this, id](bool value) {
		emit this->ButtonClicked(id, Q_XSX_JOYSTICK_ENUM::ButtonRightBumper, this->GamepadMap__[id]->buttonR1());
		});
	QObject::connect(gamepad, &QGamepad::buttonSelectChanged, this, [this, id](bool value) {
		emit this->ButtonClicked(id, Q_XSX_JOYSTICK_ENUM::ButtonSwitch, this->GamepadMap__[id]->buttonSelect());
		});
	QObject::connect(gamepad, &QGamepad::buttonStartChanged, this, [this, id](bool value) {
		emit this->ButtonClicked(id, Q_XSX_JOYSTICK_ENUM::ButtonMenu, this->GamepadMap__[id]->buttonStart());
		});
	QObject::connect(gamepad, &QGamepad::buttonCenterChanged, this, [this, id](bool value) {
		emit this->ButtonClicked(id, Q_XSX_JOYSTICK_ENUM::ButtonShare, this->GamepadMap__[id]->buttonCenter());
		});
	QObject::connect(gamepad, &QGamepad::buttonGuideChanged, this, [this, id](bool value) {
		emit this->ButtonClicked(id, Q_XSX_JOYSTICK_ENUM::ButtonXbox, this->GamepadMap__[id]->buttonGuide());
		});

	QObject::connect(gamepad, &QGamepad::buttonL3Changed, this, [this, id](bool value) {
		emit this->ButtonClicked(id, Q_XSX_JOYSTICK_ENUM::ButtonLeftStick, this->GamepadMap__[id]->buttonL3());
		});
	QObject::connect(gamepad, &QGamepad::buttonR3Changed, this, [this, id](bool value) {
		emit this->ButtonClicked(id, Q_XSX_JOYSTICK_ENUM::ButtonRightStick, this->GamepadMap__[id]->buttonR3());
		});

	QObject::connect(gamepad, &QGamepad::buttonUpChanged, this, [this, id](bool value) {
		emit this->ButtonClicked(id, Q_XSX_JOYSTICK_ENUM::ButtonPadUp, this->GamepadMap__[id]->buttonUp());
		});
	QObject::connect(gamepad, &QGamepad::buttonDownChanged, this, [this, id](bool value) {
		emit this->ButtonClicked(id, Q_XSX_JOYSTICK_ENUM::ButtonPadDown, this->GamepadMap__[id]->buttonDown());
		});
	QObject::connect(gamepad, &QGamepad::buttonLeftChanged, this, [this, id](bool value) {
		emit this->ButtonClicked(id, Q_XSX_JOYSTICK_ENUM::ButtonPadLeft, this->GamepadMap__[id]->buttonLeft());
		});
	QObject::connect(gamepad, &QGamepad::buttonRightChanged, this, [this, id](bool value) {
		emit this->ButtonClicked(id, Q_XSX_JOYSTICK_ENUM::ButtonPadRight, this->GamepadMap__[id]->buttonRight());
		});

	// QObject::connect(gamepad, &QGamepad::axisLeftXChanged, this, [this, id](double value) {
	// 	emit this->JoystickMoved(id, Q_XSX_JOYSTICK_ENUM::JoystickLeftX, -value);
	// 	});
	// QObject::connect(gamepad, &QGamepad::axisLeftYChanged, this, [this, id](double value) {
	// 	emit this->JoystickMoved(id, Q_XSX_JOYSTICK_ENUM::JoystickLeftY, -value);
	// 	});
	// QObject::connect(gamepad, &QGamepad::axisRightXChanged, this, [this, id](double value) {
	// 	emit this->JoystickMoved(id, Q_XSX_JOYSTICK_ENUM::JoystickRightX, -value);
	// 	});
	// QObject::connect(gamepad, &QGamepad::axisRightYChanged, this, [this, id](double value) {
	// 	emit this->JoystickMoved(id, Q_XSX_JOYSTICK_ENUM::JoystickRightY, -value);
	// 	});
	// QObject::connect(gamepad, &QGamepad::buttonL2Changed, this, [this, id](double value) {
	// 	emit this->JoystickMoved(id, Q_XSX_JOYSTICK_ENUM::TriggerLeft, value);
	// 	});
	// QObject::connect(gamepad, &QGamepad::buttonR2Changed, this, [this, id](double value) {
	// 	emit this->JoystickMoved(id, Q_XSX_JOYSTICK_ENUM::TriggerRight, value);
	// 	});

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