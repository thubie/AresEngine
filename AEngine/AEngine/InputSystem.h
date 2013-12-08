#pragma once

#include<Windows.h>
#include<strsafe.h>
#include"AEngine.h"

class AEngine;

class InputSystem
{
public:
    InputSystem(AEngine* controller);
    ~InputSystem();

    void RegisterDevices(HWND hWnd);
    void ProcessUserInput(RAWINPUT* rawInput);

private:
    AEngine* m_pAEngine;
};