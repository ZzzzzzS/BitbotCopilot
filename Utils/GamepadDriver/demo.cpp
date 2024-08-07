#include <QCoreApplication>
#include <iostream>
#include <vector>
#include <tuple>
#include "ZQGamepad.h"
#include <thread>

//void StateChange(DWORD id, BOOL connected)
//{
//    std::cout << "id: " << id << "connected=" << connected << std::endl;
//}
//
//void KeyChange(std::vector<std::tuple<UINT, INT, BOOL>> states)
//{
//    for (auto& state : states)
//    {
//        std::cout << "id=" << std::get<0>(state) << "button=" << std::get<1>(state) << "state=" << std::get<2>(state) << std::endl;
//        if (std::get<1>(state) == 0)
//        {
//            gp.setRumble(std::get<0>(state), 65535, 0, 500);
//        }
//    }
//}
//
//void JoystickChange(std::vector<std::tuple<UINT, INT, FLOAT>> states)
//{
//    for (auto& state : states)
//    {
//        std::cout << "id=" << std::get<0>(state) << "joystick=" << std::get<1>(state) << "state=" << std::get<2>(state) << std::endl;
//    }
//}


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    ZQGamepad handle;
    std::thread::id id_ = std::this_thread::get_id();
    std::cout << "Main ID: " << id_ << std::endl;
    QObject::connect(&handle, &ZQGamepad::ConnectionChanged,[](int id, bool Connected) {
        std::cout << "id: " << id << "connected=" << Connected << std::endl;
        });

    QObject::connect(&handle, &ZQGamepad::ButtonClicked, [](int id, Q_XSX_JOYSTICK_ENUM button, bool ButtonState) {
        std::thread::id id_ = std::this_thread::get_id();
        std::cout << "Receiver ID: " << id_ << std::endl;
        std::cout << "id: " << id << "Button="<< static_cast<int>(button)<<"state=" << ButtonState << std::endl;
        });

    QObject::connect(&handle, &ZQGamepad::JoystickMoved, [](int id, Q_XSX_JOYSTICK_ENUM button, float ButtonState) {
        std::cout << "id: " << id << "Joystick=" << static_cast<int>(button) << "state=" << ButtonState << std::endl;
        });


    handle.start();
    /*gp.RegistCallback(KeyChange, JoystickChange, StateChange);
    gp.start();*/
    _sleep(1);
    handle.SetRumble(0, 65535, 0, 500);
    return a.exec();
}
