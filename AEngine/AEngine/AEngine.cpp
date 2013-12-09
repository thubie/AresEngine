#include<assert.h>
#include"AEngine.h"



AEngine::AEngine()
{
    m_pTaskSystem       = nullptr;
    m_pRenderSystem     = nullptr;
    m_pTestRenderModel  = nullptr;
    m_pCamera           = nullptr;
    m_pGeometryManager  = nullptr;
    m_stopped = false;
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

    //Delete this later only for testing.
    m_TestTask = new CounterTask();

    m_pTaskSystem = new TaskSystem();
    if(m_pTaskSystem == nullptr)     
        return false;
    int numMaxThreads = 8;
    m_pTaskSystem->Initialize(numMaxThreads);
    m_pTaskSystem->StartDistributing();
    
    m_pInputSystem = new InputSystem(appHandle);
    if(m_pInputSystem == nullptr)
        return false;
    m_pInputSystem->RegisterDevices(m_hwnd);

    m_pRenderSystem = new RenderSystem();
    if(m_pRenderSystem == nullptr)
        return false;
    m_pRenderSystem->Initialize(m_hwnd);

    m_pCamera = new Camera();
    if(m_pCamera == nullptr)
        return false;

    //Need to refactor this..
    XMFLOAT3* pos = (XMFLOAT3*)_aligned_malloc(sizeof(XMFLOAT3),16);
    pos->x = 0.0f;
    pos->y = 70.0f;
    pos->z = -40.0f;  //-70.0

    XMFLOAT3* target = (XMFLOAT3*)_aligned_malloc(sizeof(XMFLOAT3),16);
    target->x = 0.0f;
    target->y = 50.0f;
    target->z = 0.0f;

    XMFLOAT3* up = (XMFLOAT3*)_aligned_malloc(sizeof(XMFLOAT3),16);
    up->x = 0.0f;
    up->y = 1.0f;
    up->z = 0.0f;

    m_pCamera->InitCamera(pos,target,up);
    m_pCamera->UpdateViewMatrix();

    _aligned_free(pos);
    _aligned_free(target);
    _aligned_free(up);

    m_pGeometryManager = new GeometryManager();
    m_pGeometryManager->Initialize(m_pRenderSystem->m_pD3DDevice, m_pRenderSystem->m_pImmediateContext, this);

    //Start importing asset
    m_pTaskSystem->EnqueueTask(m_pGeometryManager->ImportAssetTask("D:\\Projects\\Ares\\AresEngine\\AEngine\\Debug\\Content\\dude.dae"));

    while(!m_pGeometryManager->DoneImporting())
    {
        bool done = false;
        bool test = true;
    }
    //m_pTestRenderModel = m_pGeometryFactory->ImportAssetTest("D:\\Projects\\Ares\\AresEngine\\AEngine\\Debug\\Content\\dude.dae");
    return true;
}

bool AEngine::Shutdown()
{
    if(m_pTestRenderModel != nullptr)
        m_pTestRenderModel->CleanUpModelData();
    m_pTestRenderModel = nullptr;

    if(m_pTaskSystem != nullptr)
        m_pTaskSystem->Shutdown();
    m_pTaskSystem = nullptr;

    if(m_pInputSystem != nullptr)
        delete m_pInputSystem;
    m_pInputSystem = nullptr;

    if(m_pRenderSystem != nullptr)
        m_pRenderSystem->Shutdown();
    m_pRenderSystem = nullptr;
    m_stopped = true;
    return true;
}

void AEngine::MoveCameraForward()
{
    m_pCamera->MoveCameraForward();
}

void AEngine::MoveCameraBackward()
{
    m_pCamera->MoveCameraBackward();
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
            if (!m_stopped) 
            {
                m_pCamera->UpdateViewMatrix();
                m_pRenderSystem->BeginRenderScene();
                m_pRenderSystem->RenderScene(m_pGeometryManager->m_pMeshCollection,m_pGeometryManager->m_count, m_pCamera);
                m_pRenderSystem->EndRenderScene();           
            }
        }
    }
    Shutdown();   
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
    //ShowCursor(false);
    SetForegroundWindow(m_hwnd);
    SetFocus(m_hwnd);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
    switch(msg)
    {
        //Check if the window is being destroyed.
        case WM_DESTROY:
        {
            PostQuitMessage(0);
            return 0;
        }
        //Check if the window is being closed.
        case WM_CLOSE:
        {
            PostQuitMessage(0);
            return 0;
        }
        //Process user input messages
        case WM_INPUT:
        {
            char buffer[sizeof(RAWINPUT)] = {};
            unsigned int size = sizeof(RAWINPUT);

            GetRawInputData((HRAWINPUT)lparam, RID_INPUT,
                buffer, &size, sizeof(RAWINPUTHEADER));

            RAWINPUT* raw = (RAWINPUT*)buffer;
            appHandle->m_pInputSystem->ProcessUserInput(raw);
        }

        //All other message pass to the message handler
        default:
        {
            return DefWindowProc(hwnd, msg, wparam, lparam);
        }
    }
}
