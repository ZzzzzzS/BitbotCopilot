#pragma once

#include <QObject>
#include <QSet>
#include <QMap>
#include "QGamepad"
#include "QGamepadManager"
#include "tuple"

namespace zzs
{
    class ZGAMEPAD_XINPUT;
};


enum class Q_XSX_JOYSTICK_ENUM
{
    Button_A,//0
    Button_B,//1
    Button_X,//2
    Button_Y,//3

    ButtonLeftBumper,//4
    ButtonRightBumper,//5

    ButtonSwitch,//6
    ButtonMenu,//7
    ButtonXbox,
    ButtonShare,

    ButtonLeftStick,//8
    ButtonRightStick,//9

    ButtonPadUp,//10
    ButtonPadDown,//11
    ButtonPadLeft,//12
    ButtonPadRight,//13


    JoystickLeftX,//14
    JoystickLeftY,//15
    JoystickRightX,//16
    JoystickRightY,//17
    TriggerLeft,//18
    TriggerRight,//19
};

class ZQGamepad : public QObject
{
    Q_OBJECT

public:
    ZQGamepad(uint cps = 125, QObject* parent = nullptr);
    ~ZQGamepad();

    void start();
    bool SetRumble(int id, quint16 LeftMotor, quint16 RightMotor, quint32 Duration);
    size_t CountConnectedGamepad();
    QSet<size_t> getAvailableGamepadID();
    QString name(const Q_XSX_JOYSTICK_ENUM& key);

signals:
    void ConnectionChanged(int id, bool Connected);
    void ButtonClicked(int id, Q_XSX_JOYSTICK_ENUM button, int ButtonState);
    void JoystickMoved(int id, Q_XSX_JOYSTICK_ENUM axis, float value);
    void JoysticksMoved(QVector<std::tuple<int, Q_XSX_JOYSTICK_ENUM, double>> states);

private:

#ifdef USE_DIRECT_XINPUT
    /*void ConnectionChangeCallback(DWORD id, BOOL connected);
    void ButtonChangeCallback(std::vector<std::tuple<UINT, INT, BOOL>> states);
    void JoystickMoveCallback(std::vector<std::tuple<UINT, INT, FLOAT>> states);*/
    zzs::ZGAMEPAD_XINPUT* GamepadBackendXInput;
#endif // QT_OS_WIN

    void InitButtonNameMap();
    QMap<Q_XSX_JOYSTICK_ENUM, QString> GamepadButtonNameMap;

    QGamepadManager* GamepadManager__;
    QMap<int, QGamepad*> GamepadMap__;
    QMap<int, std::array<double, 6>> GamepadAxisValueMap__;
    QTimer* GamepadRefreshTimer__;

    void RefreshJoystickStates();
    void RefreshConnectedGamepad();
    void RegisterGamepad(int id);

};
