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
    InitializeWin();
   
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

    m_messageQueue = new RingBufferQueue<Message>(2048);

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

    m_pRenderSystem = new RenderSystem(this);
    if(m_pRenderSystem == nullptr)
        return false;

    m_pRenderSystem->Initialize(m_hwnd);

    m_pGameTimer = new HRTimer();
    
    //Need to clean up this code.
    m_pCamera = new Camera();
    if(m_pCamera == nullptr)
        return false;

    XMFLOAT3 camPosition(450.0f, 300.0f, 1000.0f); 
    XMFLOAT3 camTarget(450.0f, 30.0f, 450.0f); 
    XMFLOAT3 camUpDir(0.0f, 1.0f, 0.0f); 

    m_pCamera->InitCamera(camPosition, camTarget, camUpDir);
    m_pCamera->UpdateViewMatrix();

    m_pGeometryManager = new GeometryManager(this);
    m_pGeometryManager->Initialize(m_pRenderSystem->m_pD3DDevice, m_pRenderSystem->m_pImmediateContext);
    m_pTextureManager = new TextureManager(m_pRenderSystem->m_pD3DDevice,m_pRenderSystem->m_pImmediateContext, this);
    m_pShaderManager = new ShaderManager(m_pRenderSystem->m_pD3DDevice, m_pRenderSystem->m_pImmediateContext, this);
    m_pAnimationManager = new AnimationManager(this);
    
    //testing code
    std::vector<unsigned int> TestGameObjects;
    for(int i = 0; i < 400; ++i)
    {
        TestGameObjects.push_back(i);
    }
    m_pAnimationManager->RegisterGameObjects(TestGameObjects);
    m_CreatedTasks = m_pAnimationManager->CreateAnimationTasks(); 
    m_pRenderSystem->RegisterGameObjects(TestGameObjects);

    //Start importing asset
    m_pTaskSystem->EnqueueTask(m_pGeometryManager->ImportAssetTask(modelsDir));
    m_pTaskSystem->EnqueueTask(m_pTextureManager->ImportTextures(texturesDir));
    m_pTaskSystem->EnqueueTask(m_pShaderManager->CreateVertexShaderTask(ShaderDir));
    m_pTaskSystem->EnqueueTask(m_pShaderManager->CreatePixelShaderTask(ShaderDir));
    m_pTaskSystem->EnqueueTask(m_pAnimationManager->ImportTask(modelsDir));

    MSG msg = {0};
    m_ImportingBitfield = 0;
    bool GeometryDone = false;
    bool AnimationDone = false;

    //Polling importing state.
    while(!(GeometryDone && AnimationDone))
    {
        GeometryDone = m_pGeometryManager->DoneImporting();
        AnimationDone = m_pAnimationManager->ImportingDone;
        if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        ProcessMessageQueue();
        Sleep(1);      
    }
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

void AEngine::SubmitMessage(Message message)
{
    m_messageQueue->Enqueue(message);
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
    m_pStopWatch = new HRTimer();
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
                ProcessMessageQueue();
                Sleep(1);              
            }
        }
    }
    m_pGameTimer->Stop();
    Shutdown();   
}

//Processing the MessageQueue
void AEngine::ProcessMessageQueue()
{
    Message currentMessage;
    unsigned int numMessages = m_messageQueue->GetCount();
    float frameTime = 0.0f;
    float deltaTime = 0.0f;

    for(unsigned int i = 0; i < numMessages; ++i)
    {
        currentMessage = m_messageQueue->Dequeue();
        switch(currentMessage.MessageType)
        {
            case ANIMATIONS_DONE:
            {   
                m_pStopWatch->Stop();
                frameTime = m_pStopWatch->GetElapsedAsSeconds();
                frameTime *= 1000.0f;
                SetWindowTitle(frameTime);
                m_pRenderSystem->RenderScene(m_pGeometryManager,m_pTextureManager, m_pShaderManager, m_pCamera, m_pAnimationManager);
                break;
            }

            case RENDERING_DONE:
            {
                deltaTime = (float)m_pGameTimer->GetDeltaTime();
                m_pCamera->UpdateViewMatrix();
                m_pAnimationManager->UpdateAnimationTime(deltaTime);
                SubmitTask(m_CreatedTasks);
                m_pStopWatch->Start();
                break;
            }

            case  IMPORT_TEXTURE_DONE:
            {
                m_ImportingBitfield += IMPORT_TEXTURE_DONE;
                if(m_ImportingBitfield == IMPORTING_DONE)
                {
                    Message doneImporting;
                    doneImporting.MessageType = IMPORTING_DONE;
                    SubmitMessage(doneImporting);
                }
                break;
            }

            case IMPORT_GEOMETRY_DONE:
            {
                m_ImportingBitfield += IMPORT_GEOMETRY_DONE;
                if(m_ImportingBitfield == IMPORTING_DONE)
                {
                    Message doneImporting;
                    doneImporting.MessageType = IMPORTING_DONE;
                    SubmitMessage(doneImporting);
                }
                break;
            }

            case IMPORT_VERTEXSHADER_DONE:
            {
                m_ImportingBitfield += IMPORT_VERTEXSHADER_DONE;
                if(m_ImportingBitfield == IMPORTING_DONE)
                {
                    Message doneImporting;
                    doneImporting.MessageType = IMPORTING_DONE;
                    SubmitMessage(doneImporting);
                }
                break;
            }

            case IMPORT_PIXELSHADER_DONE:
            {
                m_ImportingBitfield += IMPORT_PIXELSHADER_DONE;
                if(m_ImportingBitfield == IMPORTING_DONE)
                {
                    Message doneImporting;
                    doneImporting.MessageType = IMPORTING_DONE;
                    SubmitMessage(doneImporting);
                }
                break;
            }

            case IMPORT_ANIMATION_DONE:
            {
                m_ImportingBitfield += IMPORT_ANIMATION_DONE;
                if(m_ImportingBitfield == IMPORTING_DONE)
                {
                    Message doneImporting;
                    doneImporting.MessageType = IMPORTING_DONE;
                    SubmitMessage(doneImporting);
                }
                break;
            }

            case IMPORTING_DONE:
            {
                m_pGameTimer->Start();
                m_pAnimationManager->UpdateAnimationTime(0.0f);
                SubmitTask(m_CreatedTasks);
                break;
            }

            default:
            {
                break;
            }
        }
    }
}

void AEngine::SetWindowTitle(float animTime)
{
    char titleStats[512]; 
    sprintf_s(titleStats,"AresEngine animTime: %f ms", animTime);
    SetWindowTextA(m_hwnd,titleStats);
}

void AEngine::SubmitTask(unsigned int createdTasks)
{
    m_openTasks = createdTasks;

    for(unsigned int i = 0; i < createdTasks; ++i)
    {
        Task* newTask = &m_pAnimationManager->m_AnimationTask[i];
        m_pTaskSystem->EnqueueTask(newTask);
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
