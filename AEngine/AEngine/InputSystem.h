#pragma once

#include<Windows.h>
#include<strsafe.h>
#include"AEngine.h"

class AEngine;

class InputSystem
{
    enum MovementKeys
    {
        FORWARD = 0x11,     //W         keyboard button
        BACKWARD =  0x1f,   //S         Keyboard button
        LEFT = 0x1e,        //A         Keyboard button
        RIGHT = 0x20,       //D         Keyboard button
        EXIT = 0x01,        //Escape    keyboard button
    };

public:
    InputSystem(AEngine* controller);
    ~InputSystem();

    void RegisterDevices(HWND hWnd);
    void ProcessUserInput(RAWINPUT* rawInput);
    void SetMovementButtons();
private:
    void ProcessKeyboardInput(RAWINPUT* KeyboardInput);
    void ProcessMouseInput(RAWINPUT* MouseInput);
    void ProcessControllerInput(RAWINPUT* ControllerInput);     //Implement this later.

private:
    AEngine* m_pAEngine;
    bool m_movementBtnStates[4];
    unsigned short m_movementScancodes[4];
};