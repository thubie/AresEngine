#include<assert.h>
#include"AEngine.h"



AEngine::AEngine()
{
    m_pTaskManager = nullptr;
    m_windowWidth = 640;
    m_windowHeight = 480;
}

AEngine::AEngine(const AEngine& other)
{
    m_windowWidth = other.m_windowWidth;
    m_windowHeight = other.m_windowHeight;
    m_pTaskManager = other.m_pTaskManager;
        
}

AEngine::~AEngine()
{
    //Delete stuff later in shutdown function
}

bool AEngine::Initialize()
{
    bool result;

    InitializeWin();
    m_pTaskManager = new TaskManager();
    if(m_pTaskManager == nullptr)     
        return false;

    result = m_pTaskManager->Initialize();
    if(!result)
        return false; //return false if Initialize fails
       
    return true;
}

bool AEngine::Shutdown()
{
    if(m_pTaskManager != nullptr)
        m_pTaskManager->Shutdown();

    m_pTaskManager = nullptr;

    return true;
}

void AEngine::Run()
{
    MSG msg = {0};
    
    while(WM_QUIT != msg.message)
    {
        if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
            //Update the world...
            for(int i = 0; i <= 8; ++i)
            {
                m_TestTask = new CounterTask();
                m_pTaskManager->EnqueueTask((ITask*)m_TestTask);
            }
            Sleep(16);
        }
            
    }
        
}

void AEngine::InitializeWin()
{
    WNDCLASS wc;
    int posX,posY;

    //Get an external pointer to this object
    appHandle = this;

    //Get the instance of this application
    m_hInstance = GetModuleHandle(NULL);

    //Set the application name.
    m_appName = L"Ares Engine";

    //Setup the window class with default settings
    memset(&wc,0,sizeof(wc));
    wc.style            = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    wc.lpfnWndProc      = WndProc;
    wc.cbClsExtra       = 0;
    wc.cbWndExtra       = 0;
    wc.hInstance        = m_hInstance;
    wc.hIcon            = LoadIcon(NULL,IDI_WINLOGO);
    wc.hCursor          = LoadCursor(NULL,IDC_ARROW);
    wc.hbrBackground    = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wc.lpszMenuName     = NULL;
    wc.lpszClassName    = m_appName;

    //register the window class
    RegisterClass(&wc);

    posX = (GetSystemMetrics(SM_CXSCREEN) - m_windowWidth) / 2;
    posY = (GetSystemMetrics(SM_CYSCREEN) - m_windowHeight) / 2;

    //Create the window with the screen setting and get the handle to it
    m_hwnd = CreateWindow(m_appName, TEXT("Ares Engine"), WS_OVERLAPPEDWINDOW, 
        posX, posY, m_windowWidth, m_windowHeight, NULL, NULL, m_hInstance, NULL);

    //Bring the window up on the screen and set it as main focus.
    ShowWindow(m_hwnd,SW_SHOW);
    SetForegroundWindow(m_hwnd);
    SetFocus(m_hwnd);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
    switch(msg)
    {
        //Check if escape has been pressed.
        case WM_KEYDOWN:
        {
            if(wparam == VK_ESCAPE)
            {
                //Need to handle this in an better...
                appHandle->Shutdown();
                DestroyWindow(hwnd);
            }
        }
        //Check if the window is being destroyed.
        case WM_DESTROY:
        {
            //Should implement shut off procedure here 
            PostQuitMessage(0);
            return 0;
        }
        //Check if the window is being closed.
        case WM_CLOSE:
        {
            PostQuitMessage(0);
            return 0;
        }
        //All other message pass to the message handler
        default:
        {
            return DefWindowProc(hwnd, msg, wparam, lparam);
        }
    }
}
