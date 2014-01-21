#include"InputSystem.h"

InputSystem::InputSystem(AEngine* controller)
{
    this->m_pAEngine = controller;
}

InputSystem::~InputSystem()
{}

void InputSystem::SetMovementButtons()
{
    //Need to adjust this later to read in input.ini file to set the codes.
    for(int i = 0; i < 4; i++)
    {
        m_movementBtnStates[i] = false; //true is pushed, false is not pushed 
    }

    m_movementScancodes[0] = FORWARD;   //W button
    m_movementScancodes[1] = BACKWARD;  //S button
    m_movementScancodes[2] = LEFT;      //A button
    m_movementScancodes[3] = RIGHT;     //D button
}

void InputSystem::RegisterDevices(HWND hWnd)
{
    RAWINPUTDEVICE devices[2];
    devices[0].usUsagePage = 0x01;
    devices[0].usUsage = 0x06;              //Keyboard 
    devices[0].dwFlags = RIDEV_NOLEGACY;    //Do not generate legacy messages such as WM_KEYDOWN
    devices[0].hwndTarget = hWnd;

    devices[1].usUsagePage = 0x01;
    devices[1].usUsage = 0x02;             //Mouse
    devices[1].dwFlags = RIDEV_NOLEGACY;   //Adds HID mouse and also ignores legacy mouse messages
    devices[1].hwndTarget = hWnd;

    RegisterRawInputDevices(devices, 2, sizeof(devices[0]));
}

void InputSystem::ProcessUserInput(RAWINPUT* rawInput)
{
    if(rawInput->header.dwType == RIM_TYPEKEYBOARD)
    {
        this->ProcessKeyboardInput(rawInput);    
    }

    if(rawInput->header.dwType == RIM_TYPEMOUSE)
    {
        this->ProcessMouseInput(rawInput);
    }

}

void InputSystem::ProcessKeyboardInput(RAWINPUT* KeyboardInput)
{
    unsigned short makeCode = KeyboardInput->data.keyboard.MakeCode;
    unsigned short flags = KeyboardInput->data.keyboard.Flags;

    switch(makeCode)
    {
        case FORWARD:
        {
            if(flags == 0x00) 
                m_movementBtnStates[0] = true;

            if(flags == 0x01)
                m_movementBtnStates[0] = false;

            break;
        }

        case BACKWARD:
        {
            if(flags == 0x00) 
                m_movementBtnStates[1] = true;

            if(flags == 0x01)
                m_movementBtnStates[1] = false;

            break;
        }

        case LEFT:
        {
            if(flags == 0x00) 
                m_movementBtnStates[2] = true;

            if(flags == 0x01)
                m_movementBtnStates[2] = false;

            break;
        }

        case RIGHT:
        {
            if(flags == 0x00) 
                m_movementBtnStates[3] = true;

            if(flags == 0x01)
                m_movementBtnStates[3] = false;
            break;
        }
            
        case EXIT:
        {
            PostQuitMessage(0);
        }
    }

    //Testcode to discover the scancodes for keyboards....!!!
    //hr = StringCchPrintf(tempString, STRSAFE_MAX_CCH, TEXT("keyBoard: make=%04x Flags:%04x Reserved:%04x, msg=%04x VK=%04x \n"),
    //    rawInput->data.keyboard.MakeCode, 
    //    rawInput->data.keyboard.Flags, 
    //    rawInput->data.keyboard.Reserved, 
    //    rawInput->data.keyboard.Message, 
    //    rawInput->data.keyboard.VKey);

    //if(FAILED(hr))
    //{}

    //OutputDebugString(tempString);
}

void InputSystem::ProcessMouseInput(RAWINPUT* MouseInput)
{
    unsigned short usFlag = MouseInput->data.mouse.usFlags;
    unsigned long ulBtn = MouseInput->data.mouse.ulButtons;

    TCHAR tempString[1024];
    HRESULT hr;
    hr = StringCchPrintf(tempString, STRSAFE_MAX_CCH, TEXT("Mouse: usFlags=%04x ulButtons=%04x usButtonFlags=%04x usButtonData=%04x ulRawButtons=%04x lLastX=%04x lLastY=%04x ulExtraInformation=%04x\r\n"), 
            MouseInput->data.mouse.usFlags, 
            MouseInput->data.mouse.ulButtons, 
            MouseInput->data.mouse.usButtonFlags, 
            MouseInput->data.mouse.usButtonData, 
            MouseInput->data.mouse.ulRawButtons, 
            MouseInput->data.mouse.lLastX, 
            MouseInput->data.mouse.lLastY, 
            MouseInput->data.mouse.ulExtraInformation);

	if (FAILED(hr))
	{
	// TODO: write error handler
	}

    if(MouseInput->data.mouse.ulButtons == 0x001)
    {
        OutputDebugString(L"Left mouse button clicked\n");
    }

    if(MouseInput->data.mouse.ulButtons == 0x002)
    {
        OutputDebugString(L"Left mouse button released\n");
    }

    if(MouseInput->data.mouse.ulButtons == 0x004)
    {
        OutputDebugString(L"Right mouse button clicked\n");
    }

    if(MouseInput->data.mouse.ulButtons == 0x008)
    {
        OutputDebugString(L"Right mouse button released\n");
    }

    /*if(rawInput->data.mouse.ulButtons == 0x0010)
    {
        OutputDebugString(L"Middle mouse button clicked\n");
    }

    if(rawInput->data.mouse.ulButtons == 0x0020)
    {
        OutputDebugString(L"Middle mouse button released\n");
    }*/

    if(MouseInput->data.mouse.ulButtons == 0x780400)
    {
        OutputDebugString(L"Mouse wheel up\n");
        m_pAEngine->MoveCameraForward();
    }

    if(MouseInput->data.mouse.ulButtons == 0xff880400)
    {
        OutputDebugString(L"Mouse wheel down\n");
        m_pAEngine->MoveCameraBackward();
    }

   /* if(!rawInput->data.mouse.ulButtons == 0x0)
    {
        OutputDebugString(tempString);
    }*/
}
