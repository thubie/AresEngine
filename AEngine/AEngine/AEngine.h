#pragma once

#include<Windows.h>
#include<assert.h>
#include"TaskSystem.h"
#include"Camera.h"
#include"RenderSystem.h"
#include"InputSystem.h"
#include"HRTimer.h"
#include"AnimationManager.h"
#include"SkeletonBuffer.h"
#include"MessageQueue.h"
#include"Message.h"

class AnimationManager;
class InputSystem;
class RenderSystem;


class AEngine
{
public:
    AEngine();
    ~AEngine();

    bool Initialize();
    void Run();
    bool Shutdown();
    void SubmitMessage(Message);
    void SubmitImportingTask(Task* importingTask);

private:
    bool Draw();
    void InitializeWin();
    void SubmitAnimationTasks(unsigned int createdTasks);    
    void ProcessMessageQueue();
    void SetWindowTitle(float time);

private:
    LPCWSTR m_appName;
    HINSTANCE m_hInstance;
    HWND m_hwnd;
    TaskSystem* m_pTaskSystem;
    RenderSystem* m_pRenderSystem;
    HRTimer* m_pGameTimer;
    HRTimer* m_pStopWatch;
    Camera* m_pCamera;
    AnimationManager* m_pAnimationManager;
    MessageQueue* m_pMessageQueue;
    unsigned int m_ImportingBitfield;
    unsigned int m_CreatedTasks;
    unsigned int m_windowWidth;
    unsigned int m_windowHeight;
    bool m_stopped;
    
public:
    InputSystem* m_pInputSystem;
};

static LRESULT CALLBACK WndProc(HWND,UINT,WPARAM,LPARAM);

static AEngine* appHandle = nullptr;

