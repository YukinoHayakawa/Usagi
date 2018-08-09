#include "GamepadButtonCode.hpp"

const char * usagi::to_string(GamepadButtonCode code)
{
    switch(code)
    {
        case GamepadButtonCode::LEFT_PAD_UP:
            return "LeftPadUp";
        case GamepadButtonCode::LEFT_PAD_DOWN:
            return "LeftPadDown";
        case GamepadButtonCode::LEFT_PAD_LEFT:
            return "LeftPadLeft";
        case GamepadButtonCode::LEFT_PAD_RIGHT:
            return "LeftPadRight";
        case GamepadButtonCode::RIGHT_PAD_UP:
            return "RightPadUp";
        case GamepadButtonCode::RIGHT_PAD_DOWN:
            return "RightPadDown";
        case GamepadButtonCode::RIGHT_PAD_LEFT:
            return "RightPadLeft";
        case GamepadButtonCode::RIGHT_PAD_RIGHT:
            return "RightPadRight";
        case GamepadButtonCode::LEFT_SHOULDER:
            return "LeftShoulder";
        case GamepadButtonCode::RIGHT_SHOULDER:
            return "RightShoulder";
        case GamepadButtonCode::LEFT_STICK:
            return "LeftStick";
        case GamepadButtonCode::RIGHT_STICK:
            return "RightStick";
        case GamepadButtonCode::SETTING_LEFT:
            return "SettingLeft";
        case GamepadButtonCode::SETTING_RIGHT:
            return "SettingRight";
        default: return "Unknown";
    }
}
