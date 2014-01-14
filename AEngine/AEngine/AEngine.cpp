#include<assert.h>
#include"AEngine.h"

AEngine::AEngine()
{
    m_pTaskSystem       = nullptr;
    m_pRenderSystem     = nullptr;
    m_pCamera           = nullptr;
    m_pGeometryManager  = nullptr;
    m_pShaderManager    = nullptr;
    m_pTextureManager   = nullptr;
    m_pAnimationManager = nullptr;
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

    m_pGameTimer = new HRTimer();
    m_pStopWatch = new HRTimer();

    //Need to clean up this code.
    m_pCamera = new Camera();
    if(m_pCamera == nullptr)
        return false;

    //Need to refactor this..
    XMFLOAT3* pos = (XMFLOAT3*)_aligned_malloc(sizeof(XMFLOAT3),16);
    pos->x = 180.0f;
    pos->y = 150.0f;
    pos->z = 450.0f;  //-70.0

    XMFLOAT3* target = (XMFLOAT3*)_aligned_malloc(sizeof(XMFLOAT3),16);
    target->x = 180.0f;
    target->y = 30.0f;
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
    m_pGeometryManager->Initialize(m_pRenderSystem->m_pD3DDevice, m_pRenderSystem->m_pImmediateContext);
    m_pTextureManager = new TextureManager(m_pRenderSystem->m_pD3DDevice,m_pRenderSystem->m_pImmediateContext);
    m_pShaderManager = new ShaderManager(m_pRenderSystem->m_pD3DDevice, m_pRenderSystem->m_pImmediateContext);
    m_pAnimationManager = new AnimationManager();

    char currentDir[1024];
    char modelsDir[1024];
    char texturesDir[1024];
    char ShaderDir[1024];

    GetCurrentDirectoryA(1024, currentDir);
    
    strcpy_s(modelsDir,currentDir);
    strcat_s(modelsDir, "\\Content\\dude.dae");
    strcpy_s(texturesDir,currentDir);
    strcat_s(texturesDir,"\\Content");
    strcpy_s(ShaderDir, currentDir);
    strcat_s(ShaderDir, "\\Shaders\\StaticMesh.fx");

    //Logging testing if dir are right
    FILE* fp;
    strcat_s(currentDir, "\\Log.txt");
    fopen_s(&fp,currentDir,"w");
    fprintf(fp, currentDir);
    fprintf(fp, "\n");
    fprintf(fp, modelsDir);
    fprintf(fp, "\n");
    fprintf(fp, ShaderDir);
    fprintf(fp, "\n");
    fprintf(fp, texturesDir);
    fprintf(fp, "\n");
    fclose(fp);

    //Start importing asset
    m_pTaskSystem->EnqueueTask(m_pGeometryManager->ImportAssetTask(modelsDir));
    m_pTaskSystem->EnqueueTask(m_pTextureManager->ImportTextures(texturesDir));
    m_pTaskSystem->EnqueueTask(m_pShaderManager->CreateVertexShaderTask(ShaderDir));
    m_pTaskSystem->EnqueueTask(m_pShaderManager->CreatePixelShaderTask(ShaderDir));
    m_pTaskSystem->EnqueueTask(m_pAnimationManager->ImportTask(modelsDir));
   
    return true;
}

bool AEngine::Shutdown()
{
    if(m_pTaskSystem != nullptr)
        m_pTaskSystem->Shutdown();
    m_pTaskSystem = nullptr;

    if(m_pInputSystem != nullptr)
        delete m_pInputSystem;
    m_pInputSystem = nullptr;

    if(m_pGeometryManager != nullptr)
    {
        m_pGeometryManager->Shutdown();
        delete m_pGeometryManager;
    }
    m_pGeometryManager = nullptr;

    if(m_pTextureManager != nullptr)
    {
        m_pTextureManager->Shutdown();
        delete m_pTextureManager;
    }
    m_pTextureManager = nullptr;

    if(m_pShaderManager != nullptr)
        delete m_pShaderManager;
    m_pShaderManager = nullptr;

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
    m_pGameTimer->Start();
    double animTime = 0.0;


    bool GeometryDone = false;
    bool AnimationDone = false;

    //Polling importing state.
    while(!(GeometryDone && AnimationDone))
    {
        GeometryDone = m_pGeometryManager->DoneImporting();
        AnimationDone = m_pAnimationManager->done;
        if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        Sleep(1);
    }

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
                ElapsedGameTime = (float)m_pGameTimer->GetGameRunTime();
                m_pAnimationManager->UpdateAnimationTest(ElapsedGameTime);
                m_pStopWatch->Start();
                m_pRenderSystem->RenderScene(m_pGeometryManager,m_pTextureManager, m_pShaderManager, m_pCamera, m_pAnimationManager);
                m_pStopWatch->Stop();
                animTime = m_pStopWatch->GetElapsedAsSeconds() * 1000.0f;
                bool finish = true;
            }
        }
    }
    m_pGameTimer->Stop();
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
