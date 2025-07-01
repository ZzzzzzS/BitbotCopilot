#pragma once
#include <Windows.h>
#include <Xinput.h>
#include <thread>
#include <iostream>
#include <memory>
#include <vector>
#include <tuple>
#include <functional>
#include <set>
#include <map>
#include <mutex>
#include <array>
#include <chrono>

namespace zzs
{
    constexpr size_t BUTTON_NUM = 14;
    constexpr size_t JOYSTICK_NUM = 6;
    constexpr FLOAT JOYSTICK_RESOLUTION = 0.01;
    class ZGAMEPAD_XINPUT
    {
    public:
        enum class XSX_JOYSTICK_ENUM
        {
            Button_A,//0
            Button_B,//1
            Button_X,//2
            Button_Y,//3

            ButtonLeftBumper,//4
            ButtonRightBumper,//5

            ButtonSwitch,//6
            ButtonMenu,//7

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

        using BUTTON_CLICKED_CALLBACK = std::function<void(std::vector<std::tuple<UINT, INT, BOOL>>)>;
        using JOYSTICK_MOVE_CALLBACK = std::function<void(std::vector<std::tuple<UINT, INT, FLOAT>>)>;
        using GAMEPAD_STATE_CHANGED_CALLBACK = std::function<void(DWORD, BOOL)>;
    public:
        ZGAMEPAD_XINPUT(UINT FPS)
            :CallingRate__(1000 / FPS),
            JoystickCallingDivider__(25),
            GamepadStateChangedCallback__(nullptr),
            ButtonClickedCallback__(nullptr),
            JoystickMoveCallback__(nullptr),
            Running__(false),
            Thread__(nullptr) {
        }

        ~ZGAMEPAD_XINPUT()
        {
            if (this->Thread__ != nullptr)
            {
                this->Running__ = false;
                this->Thread__->join();
            }
        }

        void start()
        {
            if (this->Thread__ != nullptr)
                return;

            this->Running__ = true;
            this->Thread__ = std::make_unique<std::thread>(&ZGAMEPAD_XINPUT::RUN, this);
        }

        BOOL isConnected(DWORD id)
        {
            const std::lock_guard<std::mutex> lock(this->GamepadStateLock);
            return this->AvailableGamepad__.count(id) != 0 ? true : false;
        }

        std::set<DWORD> getAvailableGamepadID()
        {
            const std::lock_guard<std::mutex> lock(this->GamepadStateLock);
            return this->AvailableGamepad__;
        }

        bool getGamepadState(DWORD id, std::array<bool, BUTTON_NUM>& ButtonState, std::array<FLOAT, 6>& JoystickState)
        {
            const std::lock_guard<std::mutex> lock(this->GamepadStateLock);
            if (this->AvailableGamepad__.count(id) == 0)
                return false;

            ButtonState = this->toButtonArray(this->GamepadStates__[id].wButtons);
            JoystickState = this->ScaleRemoveDeathZone(this->GamepadStates__[id]);
            return true;
        }

        void RegistCallback(
            BUTTON_CLICKED_CALLBACK  ButtonClicked,
            JOYSTICK_MOVE_CALLBACK JoystickMoved,
            GAMEPAD_STATE_CHANGED_CALLBACK StateChanged
        )
        {
            this->ButtonClickedCallback__ = ButtonClicked;
            this->JoystickMoveCallback__ = JoystickMoved;
            this->GamepadStateChangedCallback__ = StateChanged;
        }

        bool setRumble(DWORD id, USHORT LeftMotor, USHORT RightMotor, DWORD Duration = 500)
        {
            if (!this->Running__)
                return false;

            const std::lock_guard<std::mutex> lock(this->GamepadStateLock);
            if (this->AvailableGamepad__.count(id) == 0)
                return false;

            auto now = std::chrono::high_resolution_clock::now();
            auto offset = std::chrono::milliseconds(Duration);
            auto target = now + offset;

            this->RumbleSpeed__[id].wLeftMotorSpeed = LeftMotor;
            this->RumbleSpeed__[id].wRightMotorSpeed = RightMotor;
            this->RumbleTargetTime__[id] = target;
            return true;
        }


    private:
        void RUN()
        {
            this->Running__ = true;
            int JoystickDividerCounter = 0;
            while (this->Running__)
            {
                Sleep(this->CallingRate__);

                //scan available gamepad and process
                std::vector<std::tuple<UINT, INT, BOOL>> ChangedButtonGroup;
                std::vector<std::tuple<UINT, INT, FLOAT>> ChangedJoystickGroup;

                this->GamepadStateLock.lock();
                for (DWORD i = 0; i < XUSER_MAX_COUNT; i++)
                {
                    //scan new device
                    XINPUT_STATE state;
                    ZeroMemory(&state, sizeof(XINPUT_STATE));
                    DWORD Result = XInputGetState(i, &state);
                    if (Result == ERROR_SUCCESS)
                    {
                        if (this->AvailableGamepad__.count(i) != 0)
                            goto END_NEW_DEVICE;

                        this->AddGamepad(i);

                        if (this->GamepadStateChangedCallback__ == nullptr)
                            goto END_NEW_DEVICE;
                        this->GamepadStateChangedCallback__(i, true);
                    }
                    else
                    {
                        if (this->AvailableGamepad__.count(i) == 0)
                            goto END_NEW_DEVICE;

                        this->RemoveGamepad(i);

                        if (this->GamepadStateChangedCallback__ == nullptr)
                            goto END_NEW_DEVICE;
                        this->GamepadStateChangedCallback__(i, false);
                    }
                END_NEW_DEVICE: //goto is useful sometime

                    if (Result != ERROR_SUCCESS)
                        break;

                    //process device button
                    DWORD ChangedButton = this->GamepadStates__[i].wButtons ^ state.Gamepad.wButtons;
                    auto ChangedButtonArray = this->toButtonArray(ChangedButton);
                    auto CurrentButtonArray = this->toButtonArray(state.Gamepad.wButtons);
                    for (size_t j = 0; j < BUTTON_NUM; j++)
                    {
                        if (ChangedButtonArray[j] != 0)
                        {
                            ChangedButtonGroup.emplace_back(static_cast<UINT>(i), static_cast<INT>(j), CurrentButtonArray[j]);
                        }
                    }

                    //process device joystick


                    auto CurrentJoystickStates = this->ScaleRemoveDeathZone(state.Gamepad);
                    for (size_t j = 0; j < JOYSTICK_NUM; j++)
                    {
                        if (auto err = std::abs(CurrentJoystickStates[j] - this->LastJoystickStates__[i][j]); err > JOYSTICK_RESOLUTION)
                        {
                            ChangedJoystickGroup.emplace_back(static_cast<UINT>(i), static_cast<INT>(j + 14), CurrentJoystickStates[j]);
                            JoystickDividerCounter = 0; //reset divider counter if joystick changed
                        }
                    }

                    // if some tick passed, but still no joystick changed, we will send all joystick state to ensure the remote end and UI is updated
                    if (JoystickDividerCounter == 0 && ChangedJoystickGroup.empty())
                    {
                        for (size_t j = 0; j < JOYSTICK_NUM; j++)
                        {
                            ChangedJoystickGroup.emplace_back(static_cast<UINT>(i), static_cast<INT>(j + 14), CurrentJoystickStates[j]);
                        }
                    }



                    this->LastJoystickStates__[i] = CurrentJoystickStates;
                    this->GamepadStates__[i] = state.Gamepad;
                    //process rumble
                    this->ProcessRumble(i);
                }
                this->GamepadStateLock.unlock();

                if (!ChangedButtonGroup.empty() && this->ButtonClickedCallback__ != nullptr)
                {
                    this->ButtonClickedCallback__(ChangedButtonGroup);
                }


                if (!ChangedJoystickGroup.empty() && this->JoystickMoveCallback__ != nullptr)
                {
                    this->JoystickMoveCallback__(ChangedJoystickGroup);
                }

                JoystickDividerCounter++;
                JoystickDividerCounter %= JoystickCallingDivider__;
            }
        }

        void RemoveGamepad(DWORD id)
        {
            this->AvailableGamepad__.erase(id);
            this->RumbleTargetTime__.erase(id);
            this->RumbleStates__.erase(id);
            this->GamepadStates__.erase(id);
            this->LastJoystickStates__.erase(id);
            this->RumbleSpeed__.erase(id);
        }

        void AddGamepad(DWORD id)
        {
            this->AvailableGamepad__.insert(id);
            this->RumbleTargetTime__.insert(std::make_pair(id, std::chrono::high_resolution_clock::now()));
            this->RumbleStates__.insert(std::make_pair(id, false));
            XINPUT_GAMEPAD NewGamepad;
            ZeroMemory(&NewGamepad, sizeof(XINPUT_GAMEPAD));
            this->GamepadStates__.insert(std::make_pair(id, NewGamepad));
            auto JoystickState = this->ScaleRemoveDeathZone(NewGamepad);
            this->LastJoystickStates__.insert(std::make_pair(id, JoystickState));

            XINPUT_VIBRATION vibration;
            ZeroMemory(&vibration, sizeof(XINPUT_VIBRATION));
            this->RumbleSpeed__.insert(std::make_pair(id, vibration));
        }

        std::array<FLOAT, JOYSTICK_NUM> ScaleRemoveDeathZone(const XINPUT_GAMEPAD& GamepadState)
        {
            auto [lx, ly] = this->RemoveJoystickDeadZone(GamepadState.sThumbLX, GamepadState.sThumbLY, XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE / 10);
            auto [rx, ry] = this->RemoveJoystickDeadZone(GamepadState.sThumbRX, GamepadState.sThumbRY, XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE / 10);
            auto lt = this->RemoveTriggerDeadZone(GamepadState.bLeftTrigger, XINPUT_GAMEPAD_TRIGGER_THRESHOLD / 6);
            auto rt = this->RemoveTriggerDeadZone(GamepadState.bRightTrigger, XINPUT_GAMEPAD_TRIGGER_THRESHOLD / 6);
            std::array<FLOAT, JOYSTICK_NUM> arr = { lx,ly,rx,ry,lt,rt };
            return arr;
        }

        std::array<bool, BUTTON_NUM> toButtonArray(const DWORD& GamepadButton)
        {
            std::array<bool, BUTTON_NUM> states = { false };
            states[0] = GamepadButton & XINPUT_GAMEPAD_A;
            states[1] = GamepadButton & XINPUT_GAMEPAD_B;
            states[2] = GamepadButton & XINPUT_GAMEPAD_X;
            states[3] = GamepadButton & XINPUT_GAMEPAD_Y;
            states[4] = GamepadButton & XINPUT_GAMEPAD_LEFT_SHOULDER;
            states[5] = GamepadButton & XINPUT_GAMEPAD_RIGHT_SHOULDER;
            states[6] = GamepadButton & XINPUT_GAMEPAD_START;
            states[7] = GamepadButton & XINPUT_GAMEPAD_BACK;
            states[8] = GamepadButton & XINPUT_GAMEPAD_LEFT_THUMB;
            states[9] = GamepadButton & XINPUT_GAMEPAD_RIGHT_THUMB;
            states[10] = GamepadButton & XINPUT_GAMEPAD_DPAD_UP;
            states[11] = GamepadButton & XINPUT_GAMEPAD_DPAD_DOWN;
            states[12] = GamepadButton & XINPUT_GAMEPAD_DPAD_LEFT;
            states[13] = GamepadButton & XINPUT_GAMEPAD_DPAD_RIGHT;
            return states;
        }

        std::tuple<FLOAT, FLOAT> RemoveJoystickDeadZone(SHORT x, SHORT y, SHORT DeadThreshold)
        {
            FLOAT Magnitude = std::sqrt(std::pow(static_cast<FLOAT>(x), 2.0) + std::pow(static_cast<FLOAT>(y), 2.0));
            FLOAT NormalizedMagnitude = 0;

            if (Magnitude > DeadThreshold)
            {
                if (Magnitude > SHRT_MAX)
                    Magnitude = SHRT_MAX;
                NormalizedMagnitude = (Magnitude - DeadThreshold) / (SHRT_MAX - DeadThreshold);

                FLOAT NormalizedLX = x / Magnitude;
                FLOAT NormalizedLY = y / Magnitude;
                NormalizedLX *= NormalizedMagnitude;
                NormalizedLY *= NormalizedMagnitude;
                return std::make_tuple(NormalizedLX, NormalizedLY);
            }
            else
            {
                return std::make_tuple<FLOAT, FLOAT>(0.0, 0.0);
            }
        }

        FLOAT RemoveTriggerDeadZone(BYTE x, BYTE DeadThreshold)
        {
            if (x < DeadThreshold)
            {
                return 0;
            }
            else
            {
                return(static_cast<FLOAT>(x) - static_cast<FLOAT>(DeadThreshold)) /
                    (static_cast<FLOAT>(UCHAR_MAX) - static_cast<FLOAT>(DeadThreshold));
            }
        }

        void ProcessRumble(DWORD id)
        {
            auto now = std::chrono::high_resolution_clock::now();
            if (this->RumbleStates__[id]) //try to stop
            {
                if (now > this->RumbleTargetTime__[id])
                {
                    this->RumbleSpeed__[id].wLeftMotorSpeed = 0;
                    this->RumbleSpeed__[id].wRightMotorSpeed = 0;
                    XInputSetState(id, &this->RumbleSpeed__[id]);
                    this->RumbleStates__[id] = false;
                }
            }
            else //try to start
            {
                if (now < this->RumbleTargetTime__[id])
                {
                    XInputSetState(id, &this->RumbleSpeed__[id]);
                    this->RumbleStates__[id] = true;
                }
            }
        }

    private:
        std::unique_ptr<std::thread> Thread__;
        BOOL Running__;

        BUTTON_CLICKED_CALLBACK ButtonClickedCallback__;
        JOYSTICK_MOVE_CALLBACK JoystickMoveCallback__;
        GAMEPAD_STATE_CHANGED_CALLBACK GamepadStateChangedCallback__;

        std::set<DWORD> AvailableGamepad__;

        std::map<DWORD, std::chrono::steady_clock::time_point> RumbleTargetTime__;
        std::map<DWORD, XINPUT_VIBRATION> RumbleSpeed__;
        std::map<DWORD, BOOL> RumbleStates__;

        std::map<DWORD, XINPUT_GAMEPAD> GamepadStates__;
        std::map<DWORD, std::array<FLOAT, JOYSTICK_NUM>> LastJoystickStates__;
        std::mutex GamepadStateLock;

        CONST DWORD CallingRate__;
        CONST DWORD JoystickCallingDivider__;

    };
};