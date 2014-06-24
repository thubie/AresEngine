#include"AEngine.h"

extern AEngine* appHandle;

AEngine::AEngine()
{
    m_pTaskSystem = nullptr;
    m_pRenderSystem = nullptr;
    m_pCamera = nullptr;
    m_pAnimationManager = nullptr;
    m_pScriptManager = nullptr;
    m_stopped = false;
    m_windowWidth = 1280;
    m_windowHeight = 720;    
}

AEngine::~AEngine()
{
    //Delete stuff later in shutdown function
}

//Initalize the Engine and create subsystems
bool AEngine::Initialize()
{    
    unsigned int numThreads = 0;
    char currentDir[1024];
    appHandle = this;
    GetCurrentDirectoryA(1024, currentDir);

    m_pScriptManager = new ScriptManager(this ,currentDir);
    ExposeSystems();
    m_pScriptManager->GetConfigSetting(&m_windowWidth, &m_windowHeight, &numThreads);
    InitializeWin(m_windowWidth, m_windowHeight);
    
    m_pMessageQueue = new MessageQueue(1024);
    m_pGameTimer = new HRTimer();
    m_pStopWatch = new HRTimer();

    m_pTaskSystem = &m_pTaskSystem->GetTaskSystem(); //new TaskSystem();
    
    if(m_pTaskSystem == nullptr)     
        return false;

    m_pTaskSystem->Initialize(numThreads);
    m_pTaskSystem->StartDistributing();

    m_pInputSystem = new InputSystem(appHandle);
    if(m_pInputSystem == nullptr)
        return false;
    m_pInputSystem->RegisterDevices(m_hwnd);

    m_pRenderSystem = new RenderSystem(this);
    if(m_pRenderSystem == nullptr)
        return false;
    m_pRenderSystem->Initialize(m_hwnd, currentDir);

    m_pCamera = new Camera();
    if(m_pCamera == nullptr)
        return false;
    
    //XMFLOAT3 camPosition(30.0f, 200.0f, -200.0f); 
    //XMFLOAT3 camTarget(450.0f, 30.0f, 450.0f); 

    XMFLOAT3 camPosition(-200.0f, 200.0f, -250.0f); 
    XMFLOAT3 camTarget(600.0f, 30.0f, 450.0f); 
    XMFLOAT3 camUpDir(0.0f, 1.0f, 0.0f); 

    m_pCamera->InitCamera(camPosition, camTarget, camUpDir);
    m_pAnimationManager = new AnimationManager(this, currentDir);
    
    RegisterGameObjects(this,500);

    this->m_InitializeState = new InitializeState(this);
    this->m_Runstate = new Runstate(this);
    
    this->m_state = m_InitializeState; 
    //Start importing assets
    m_pRenderSystem->SubmitImportingTasks();
    m_pTaskSystem->EnqueueTask(m_pAnimationManager->ImportTask());
    TestSetNumThread = 5.0f;
    return true;
}

//Shutdown and delete subsystems
bool AEngine::Shutdown()
{
    m_stopped = true;

    if(m_pTaskSystem != nullptr)
    {
        m_pTaskSystem->Shutdown();
        //delete m_pTaskSystem;
    }
    m_pTaskSystem = nullptr;

    if(m_pMessageQueue != nullptr)
        delete m_pMessageQueue;
    m_pMessageQueue = nullptr;

    if(m_pInputSystem != nullptr)
        delete m_pInputSystem;
    m_pInputSystem = nullptr;

    if(m_pRenderSystem != nullptr)
    {
        m_pRenderSystem->Shutdown();
        delete m_pRenderSystem;
    }
    m_pRenderSystem = nullptr;
    
    m_stopped = true;
    return true;
}

//Register game objects
void AEngine::RegisterGameObjects(AEngine* engine,unsigned int numGameObjects)
{
    //testing code
    std::vector<unsigned int> TestGameObjects;
    for(int i = 0; i < numGameObjects; ++i)
    {
        TestGameObjects.push_back(i);
    }
    Sleep(200); //Need to find a way to wait for workerthreads to finish their current task and put in pause mode.
    //Registering game objects.
    engine->m_pRenderSystem->RegisterGameObjects(TestGameObjects);
    engine->m_pAnimationManager->RegisterGameObjects(TestGameObjects);
    engine->m_CreatedTasks = m_pAnimationManager->CreateAnimationTasks();
}

//Submit message to message queue
void AEngine::SubmitMessage(Message message)
{
    m_pMessageQueue->Enqueue(message);
}

//Run the engine
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
                //m_pScriptManager->SetRuntimeSetting(); //Need to change this to listen to new script message
                ProcessMessageQueue();
                Sleep(1);              
            }
        }
    }
    m_pGameTimer->Stop();
    Shutdown();   
}

//Change to the Runstate
void AEngine::ChangeToRunstate()
{   
    m_state = m_Runstate;
    m_pStopWatch->Start();
    m_pAnimationManager->UpdateAnimationTime(0.0f);
    SubmitAnimationTasks(m_CreatedTasks);
}

//Render the world
void AEngine::RenderWorld()
{
    m_pStopWatch->Stop();
    float frameTime = m_pStopWatch->GetElapsedAsSeconds();
    frameTime *= 1000.0f; //change seconds to ms
    unsigned int numThreads = m_pTaskSystem->GetNumThreads();
    SetWindowTitle(frameTime, numThreads);
    XMMATRIX* pView = m_pCamera->GetViewMatrix();
    XMMATRIX* pProjection = m_pCamera->GetProjectionMatrix();
    std::vector<SkeletonCBufferData>* pFinalTransforms = m_pAnimationManager->GetFinalTransforms();
    m_pRenderSystem->RenderScene(pView, pProjection, pFinalTransforms);
}

//Update the animation state of the game objects.
void AEngine::AnimateWorld()
{
    volatile float deltaTime = m_pStopWatch->GetDeltaTime();
    XMVECTOR userInput = m_pInputSystem->GetUserInput(deltaTime);
    m_pCamera->UpdateViewMatrix(userInput);
    m_pAnimationManager->UpdateAnimationTime(deltaTime);
    SubmitAnimationTasks(m_CreatedTasks);
    m_pStopWatch->Start();
}

//Execute run time script.
void AEngine::RuntimeScript()
{
    m_pScriptManager->SetRuntimeSetting();
}

//Process the MessageQueue
void AEngine::ProcessMessageQueue()
{       
    m_state->ProcessMessage(m_pMessageQueue);    
}

//Set the window title
void AEngine::SetWindowTitle(float animTime, unsigned int numThreads)
{
    char titleStats[512]; 
    sprintf_s(titleStats,"AresEngine animTime: %f ms Number of threads active: %u", animTime, numThreads);
    SetWindowTextA(m_hwnd,titleStats);
}

//Submits importing task to be processing
void AEngine::SubmitImportingTask(Task* importingTask)
{
    m_pTaskSystem->EnqueueTask(importingTask);
}

//Submit Animation Tasks to taskQueue
void AEngine::SubmitAnimationTasks(unsigned int createdTasks)
{
    for(unsigned int i = 0; i < createdTasks; ++i)
    {
        Task* newTask = &m_pAnimationManager->m_AnimationTask.at(i);
        m_pTaskSystem->EnqueueTask(newTask);
    }
    
}

TaskSystem* AEngine::GetTaskSystem()
{
    return this->m_pTaskSystem;
}

//Innitialize the Window
void AEngine::InitializeWin(int width, int heigh)
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

    posX = 0;//(GetSystemMetrics(SM_CXSCREEN) - m_windowWidth) / 2;
    posY = 0;//(GetSystemMetrics(SM_CYSCREEN) - m_windowHeight) / 2;

    //Create the window with the screen setting and get the handle to it
    m_hwnd = CreateWindow(m_appName, TEXT("Ares Engine"), WS_OVERLAPPEDWINDOW, 
        posX, posY, m_windowWidth, m_windowHeight, NULL, NULL, m_hInstance, NULL);

    //Bring the window up on the screen and set it as main focus.
    ShowWindow(m_hwnd,SW_SHOW);
    ShowCursor(false);
    SetForegroundWindow(m_hwnd);
    SetFocus(m_hwnd);
}

//Expose the sub systems to lua
void AEngine::ExposeSystems()
{
    luaL_Reg worldFunctions [] = {
        {"RegisterGameObjects", RegisterGameObjects},
        {"GetAnimationSystem", GetAnimationSystem},
        {"GetTaskSystem", GetTaskSystem},
        {NULL, NULL}
    };

    luaL_Reg TaskSystemFunctions [] = 
    {
        {"SetNumWorkerThreads", TaskSystem::SetNumThreads},
        {NULL, NULL}
    };

    luaL_Reg AnimationSystemFunctions [] = 
    {
        {"ChangeGameObjectAnimSpeed", AnimationManager::ChangeGameObjectAnimSpeed},
        {NULL, NULL}
    };

    m_pScriptManager->LuaOpenLibrary(worldFunctions, "WorldLib");
    m_pScriptManager->LuaOpenLibrary(TaskSystemFunctions, "TaskSystemLib");
    m_pScriptManager->LuaOpenLibrary(AnimationSystemFunctions, "AnimationSystemLib");
}

//Windows message processing
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

///////////////////////////////////////////////////
//Lua AEgine exposed functions
///////////////////////////////////////////////////

//Function called from lua.
int AEngine::RegisterGameObjects(lua_State* luaVM)
{
    //check if stack element is userdata
    if(!lua_islightuserdata(luaVM, -2))
    {
        
        //print error message to Visual studio debug output window.
        bool test = true;
    }
    unsigned int numGameObjects = lua_tointeger(luaVM,-1);
    AEngine* engine = (AEngine*)lua_touserdata(luaVM, -2);
    engine->RegisterGameObjects(engine, numGameObjects);
    return 0;
}

//Returns the m_pAnimationSystem as (void*)light user data to lua
int AEngine::GetAnimationSystem(lua_State* luaVM)
{
    AEngine* engine = (AEngine*)lua_touserdata(luaVM, -1);
    lua_pushlightuserdata(luaVM, (void*) engine->m_pAnimationManager);
    return 1;
}

//Returns the m_pTaskSystem as (void*)light user data to lua
int AEngine::GetTaskSystem(lua_State* luaVM)
{
    AEngine* engine = (AEngine*)lua_touserdata(luaVM, -1);
    lua_pushlightuserdata(luaVM, (void*) engine->m_pTaskSystem);
    return 1;
}