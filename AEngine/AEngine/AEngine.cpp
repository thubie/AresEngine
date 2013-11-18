#include<assert.h>
#include"AEngine.h"



AEngine::AEngine()
{
    m_pTaskSystem     = nullptr;
    m_pRenderSystem     = nullptr;
    m_pTestRenderModel  = nullptr;
    m_pCamera           = nullptr;
    m_windowWidth = 1600;
    m_windowHeight = 900;
}

AEngine::AEngine(const AEngine& other)
{
    m_windowWidth = other.m_windowWidth;
    m_windowHeight = other.m_windowHeight;
    m_pTaskSystem = other.m_pTaskSystem;
        
}

AEngine::~AEngine()
{
    //Delete stuff later in shutdown function
}

bool AEngine::Initialize()
{
    bool result;

    InitializeWin();

    m_pTaskSystem = new TaskSystem();
    if(m_pTaskSystem == nullptr)     
        return false;
    result = m_pTaskSystem->Initialize();
    if(!result)
        return false; 
    
    m_pRenderSystem = new RenderSystem();
    if(m_pRenderSystem == nullptr)
        return false;
    m_pRenderSystem->Initialize(m_hwnd);

    m_pCamera = new Camera();
    if(m_pCamera == nullptr)
        return false;
    XMFLOAT3* pos = new XMFLOAT3(0.0f, 4.5f, -10.0f);
    XMFLOAT3* target = new XMFLOAT3(0.0f, 1.0f, 0.0f);
    XMFLOAT3* up = new XMFLOAT3(0.0f, 1.0f, 0.0f);

    m_pCamera->InitCamera(pos,target,up);
    m_pCamera->UpdateViewMatrix();

    delete pos;
    delete target;
    delete up;

    m_pTestRenderModel = new Model(m_pRenderSystem->m_pImmediateContext,m_pRenderSystem->m_pD3DDevice);
    if(m_pTestRenderModel == nullptr)     
        return false;
    m_pTestRenderModel->InitModel();

    return true;
}

bool AEngine::Shutdown()
{
    if(m_pTestRenderModel != nullptr)
        m_pTestRenderModel->CleanUpModel();
    m_pTestRenderModel = nullptr;

    if(m_pTaskSystem != nullptr)
        m_pTaskSystem->Shutdown();
    m_pTaskSystem = nullptr;

    if(m_pRenderSystem != nullptr)
        m_pRenderSystem->Shutdown();
    m_pRenderSystem = nullptr;
 
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
                m_pTaskSystem->EnqueueTask((ITask*)m_TestTask);
            }
            m_pRenderSystem->BeginRenderScene();
            m_pCamera->UpdateViewMatrix();
            m_pTestRenderModel->Render(m_pCamera);
            m_pRenderSystem->EndRenderScene();
            Sleep(15);
        }
            
    }
        
}

void AEngine::SubmitTask(ITask* task)
{
    m_pTaskSystem->EnqueueTask(task);
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
