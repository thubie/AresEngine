#include"InputSystem.h"

InputSystem::InputSystem(AEngine* controller)
{
    this->m_pAEngine = controller;
}

InputSystem::~InputSystem()
{
    m_pAEngine = nullptr;
}

//returns the user input.
XMVECTOR InputSystem::GetUserInput(float deltaTime)
{
    XMVECTOR userInput;
    userInput = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
    float speed = 150.0f * deltaTime;

    if(m_movementBtnStates[0] == true)
    {
        userInput.m128_f32[1] += speed;  
    }
    if(m_movementBtnStates[1] == true)
    {
        userInput.m128_f32[1] -= speed;  
    }
    if(m_movementBtnStates[2] == true)
    {
        userInput.m128_f32[0] -= speed;  
    }
    if(m_movementBtnStates[3] == true)
    {
        userInput.m128_f32[0] += speed;  
    }

    if(m_CurrMouseInput.lLastX > 1 || m_CurrMouseInput.lLastX < -1)
    {
        userInput.m128_f32[2] = m_CurrMouseInput.lLastX * 0.6f * deltaTime;
    }
    
    if(m_CurrMouseInput.lLastY > 1 || m_CurrMouseInput.lLastY < -1)
    {
        userInput.m128_f32[3] = m_CurrMouseInput.lLastY * 0.6f * deltaTime;
    }

    

    return userInput;
}

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

void InputSystem::ProcessKeyboardInput(RAWINPUT* keyboardInput)
{
    unsigned short makeCode = keyboardInput->data.keyboard.MakeCode;
    unsigned short flags = keyboardInput->data.keyboard.Flags;

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
}

void InputSystem::ProcessMouseInput(RAWINPUT* mouseInput)
{
    m_CurrMouseInput = mouseInput->data.mouse;
}
