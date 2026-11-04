#pragma once

#include <memory>

namespace GOL {

enum ControllerButton {
    DIAMOND_TOP,
    DIAMOND_BOTTOM,
    DIAMOND_LEFT,
    DIAMOND_RIGHT,
    BACK,
    GUIDE,
    START,
    LEFT_STICK,
    RIGHT_STICK,
    LEFT_SHOULDER,
    RIGHT_SHOULDER,
    DPAD_UP,
    DPAD_DOWN,
    DPAD_LEFT,
    DPAD_RIGHT,
    MISC1,
    LEFT_PADDLE1,
    RIGHT_PADDLE1,
    LEFT_PADDLE2,
    RIGHT_PADDLE2,
    TOUCHPAD,
    MISC2,
    MISC3,
    MISC4,
    MISC5,
    MISC6,
    LS_CONTROLLER_BUTTON_COUNT
};

enum ControllerAxis {
    LEFT_X,
    LEFT_Y,
    RIGHT_X,
    RIGHT_Y,
    LEFT_PEDAL,
    RIGHT_PEDAL,
    LS_CONTROLLER_AXIS_COUNT
};

/// Class for receiving input from ALL connected controllers. Supports plug and play which is on by default.
class ControllerManager {
private:
    struct Impl;
    std::unique_ptr<Impl> m_impl;

public:

    /// Receives input states of all controllers
    void updateInputs();

    /// @returns Whether the specified button on a specified controller is currently pressed.
    bool isButtonPressed(unsigned controllerID, ControllerButton button);

    /// @returns An analog value ranging [0.f - 1.f] of a specified axis on a specified controller
    float getAxisValue(unsigned controllerID, ControllerAxis axis);

    /// @returns The amount of controllers that are currently connected to the system.
    unsigned getControllerCount();

    /// @returns Whether the specified controller is ok.
    bool controllerIsActive(unsigned controllerID);

    /// @returns The amount of buttons and axises found on a specified controller.
    unsigned getButtonCount(unsigned controllerID);

    explicit ControllerManager(bool plugAndPlay = true);
    ~ControllerManager();
    ControllerManager(const ControllerManager&) = delete;
    ControllerManager& operator=(const ControllerManager&) = delete;
};

}