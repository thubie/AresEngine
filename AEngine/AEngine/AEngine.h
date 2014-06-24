#pragma once

#include<Windows.h>
#include<assert.h>
#include"TaskSystem.h"
#include"Camera.h"
#include"RenderSystem.h"
#include"InputSystem.h"
#include"HRTimer.h"
#include"AnimationManager.h"
#include"ScriptManager.h"
#include"SkeletonBuffer.h"
#include"IState.h"
#include"InitializeState.h"
#include"Runstate.h"
#include"MessageQueue.h"
#include"Message.h"

class AnimationManager;
class InputSystem;
class RenderSystem;
class ScriptManager;
class TaskSystem;
class InitializeState;
class Runstate;

class AEngine
{
public:
    AEngine();
    ~AEngine();
    bool Initialize();
    void RegisterGameObjects(AEngine* engine, unsigned int numGameObjects);
    void Run();
    void ChangeToRunstate();
    void RenderWorld();
    void AnimateWorld();
    bool Shutdown();
    void SubmitMessage(Message);
    void SubmitImportingTask(Task* importingTask);
    TaskSystem* GetTaskSystem();
    void RuntimeScript();
    //Lua exposed library function
    static int RegisterGameObjects(lua_State* luaVM);
    static int GetAnimationSystem(lua_State* luaVM);
    static int GetTaskSystem(lua_State* luaVM);
    
private:       
    bool Draw();
    void InitializeWin(int width, int heigh);
    void ExposeSystems();
    void SubmitAnimationTasks(unsigned int createdTasks);    
    void ProcessMessageQueue();
    void SetWindowTitle(float time, unsigned int numThreads);

private:
    LPCWSTR m_appName;
    HINSTANCE m_hInstance;
    HWND m_hwnd;
    
    RenderSystem* m_pRenderSystem;
    HRTimer* m_pGameTimer;
    HRTimer* m_pStopWatch;
    Camera* m_pCamera;
    AnimationManager* m_pAnimationManager;
    ScriptManager* m_pScriptManager;
    MessageQueue* m_pMessageQueue;
    
    unsigned int m_CreatedTasks;
    unsigned int m_windowWidth;
    unsigned int m_windowHeight;
    bool m_stopped;
    InitializeState* m_InitializeState;
    Runstate* m_Runstate;
    IState* m_state;
    float TestSetNumThread;

    luaL_Reg* worldFunctions; 
   

public:
    InputSystem* m_pInputSystem;
    TaskSystem* m_pTaskSystem;
};


static AEngine* appHandle = nullptr;

static LRESULT CALLBACK WndProc(HWND,UINT,WPARAM,LPARAM);



