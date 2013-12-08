#include"InputSystem.h"

InputSystem::InputSystem(AEngine* controller)
{
    this->m_pAEngine = controller;
}

InputSystem::~InputSystem()
{}

void InputSystem::RegisterDevices(HWND hWnd)
{
    RAWINPUTDEVICE devices[2];
    devices[0].usUsagePage = 0x01;
    devices[0].usUsage = 0x06;              //Keyboard 
    devices[0].dwFlags = RIDEV_NOLEGACY;    //Do not generate legacy messages such as WM_KEYDOWN
    devices[0].hwndTarget = hWnd;

    devices[1].usUsagePage = 0x01;
    devices[1].usUsage = 0x02;              //Mouse
    devices[1].dwFlags = RIDEV_NOLEGACY;   //Adds HID mouse and also ignores legacy mouse messages
    devices[1].hwndTarget = hWnd;

    RegisterRawInputDevices(devices, 2, sizeof(devices[0]));
}


void InputSystem::ProcessUserInput(RAWINPUT* rawInput)
{
    TCHAR tempString[1024];
    HRESULT hr;

    if(rawInput->header.dwType == RIM_TYPEKEYBOARD)
    {
        hr = StringCchPrintf(tempString, STRSAFE_MAX_CCH, TEXT("keyBoard: make=%04x Flags:%04x Reserved:%04x, msg=%04x VK=%04x \n"),
            rawInput->data.keyboard.MakeCode, 
            rawInput->data.keyboard.Flags, 
            rawInput->data.keyboard.Reserved, 
            rawInput->data.keyboard.Message, 
            rawInput->data.keyboard.VKey);

        if(FAILED(hr))
        {}

        OutputDebugString(tempString);

        if(rawInput->data.keyboard.MakeCode == 0x11 && rawInput->data.keyboard.Flags == 0x0)
        {
             OutputDebugString(L"W button clicked\n");
             m_pAEngine->MoveCameraForward();
        }

        if(rawInput->data.keyboard.MakeCode == 0x11 && rawInput->data.keyboard.Flags == 0x01)
        {
             OutputDebugString(L"W button released\n");
        }

        if(rawInput->data.keyboard.MakeCode == 0x1e)
        {
             OutputDebugString(L"A button clicked\n");
        }

        if(rawInput->data.keyboard.MakeCode == 0x1f)
        {
             OutputDebugString(L"S button clicked\n");
             m_pAEngine->MoveCameraBackward();
        }

        if(rawInput->data.keyboard.MakeCode == 0x21)
        {
             OutputDebugString(L"D button clicked\n");
        }

        if(rawInput->data.keyboard.MakeCode == 0x1)
        {
            //Escape button
            PostQuitMessage(0);
        }
    }

    if(rawInput->header.dwType == RIM_TYPEMOUSE)
    {
        hr = StringCchPrintf(tempString, STRSAFE_MAX_CCH, TEXT("Mouse: usFlags=%04x ulButtons=%04x usButtonFlags=%04x usButtonData=%04x ulRawButtons=%04x lLastX=%04x lLastY=%04x ulExtraInformation=%04x\r\n"), 
            rawInput->data.mouse.usFlags, 
            rawInput->data.mouse.ulButtons, 
            rawInput->data.mouse.usButtonFlags, 
            rawInput->data.mouse.usButtonData, 
            rawInput->data.mouse.ulRawButtons, 
            rawInput->data.mouse.lLastX, 
            rawInput->data.mouse.lLastY, 
            rawInput->data.mouse.ulExtraInformation);

		if (FAILED(hr))
		{
		// TODO: write error handler
		}

        if(rawInput->data.mouse.ulButtons == 0x001)
        {
            OutputDebugString(L"Left mouse button clicked\n");
        }

        if(rawInput->data.mouse.ulButtons == 0x002)
        {
            OutputDebugString(L"Left mouse button released\n");
        }

        if(rawInput->data.mouse.ulButtons == 0x004)
        {
            OutputDebugString(L"Right mouse button clicked\n");
        }

        if(rawInput->data.mouse.ulButtons == 0x008)
        {
            OutputDebugString(L"Right mouse button released\n");
        }

        if(rawInput->data.mouse.ulButtons == 0x0010)
        {
            OutputDebugString(L"Middle mouse button clicked\n");
        }

        if(rawInput->data.mouse.ulButtons == 0x0020)
        {
            OutputDebugString(L"Middle mouse button released\n");
        }

        if(rawInput->data.mouse.ulButtons == 0x780400)
        {
            OutputDebugString(L"Mouse wheel up\n");
            m_pAEngine->MoveCameraForward();
        }

        if(rawInput->data.mouse.ulButtons == 0xff880400)
        {
            OutputDebugString(L"Mouse wheel down\n");
            m_pAEngine->MoveCameraBackward();
        }

        if(rawInput->data.mouse.ulButtons == 0x0)
        {
            OutputDebugString(tempString);
        }
        
    }

}

