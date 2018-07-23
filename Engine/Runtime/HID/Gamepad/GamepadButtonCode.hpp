#pragma once

namespace usagi
{
/**
 * \brief Corresponding buttons on XInput controller and Dualshock are
 * written in comments.
 */
enum class GamepadButtonCode
{
    LEFT_PAD_UP,
    LEFT_PAD_DOWN,
    LEFT_PAD_LEFT,
    LEFT_PAD_RIGHT,

    // Y/triangle
    RIGHT_PAD_UP,
    // A/cross
    RIGHT_PAD_DOWN,
    // X/square
    RIGHT_PAD_LEFT,
    // B/circle
    RIGHT_PAD_RIGHT,

    LEFT_SHOULDER,
    RIGHT_SHOULDER,

    LEFT_STICK,
    RIGHT_STICK,

    // back/share
    SETTING_LEFT,
    // start/option
    SETTING_RIGHT,
};

const char * to_string(GamepadButtonCode code);
}
