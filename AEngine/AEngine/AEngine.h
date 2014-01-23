#pragma once

#include<Windows.h>
#include"TaskSystem.h"
#include"Camera.h"
#include"RenderSystem.h"
#include"InputSystem.h"
#include"HRTimer.h"
#include"GeometryManager.h"
#include"ShaderManager.h"
#include"TextureManager.h"
#include"AnimationManager.h"
#include"RingBufferQueue.h"
#include"MessageQueue.h"
#include"Message.h"

class InputSystem;
class RenderSystem;
class GeometryManager;
class TextureManager;
class AnimationManager;
class ShaderManager;

class AEngine
{
public:
    AEngine();
    ~AEngine();

    bool Initialize();
    void Run();
    bool Shutdown();
    void SubmitMessage(Message);

private:
    bool Draw();
    void InitializeWin();
    void SubmitTask(unsigned int createdTasks);
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
    GeometryManager* m_pGeometryManager;
    ShaderManager* m_pShaderManager;
    TextureManager* m_pTextureManager;
    AnimationManager* m_pAnimationManager;

    RingBufferQueue<Message>* m_messageQueue;
    MessageQueue* m_pMessageQueue;
    unsigned int m_ImportingBitfield;
    unsigned int m_CreatedTasks;
    unsigned int m_windowWidth; //To do read window resolution from a config/ini file
    unsigned int m_windowHeight;
    bool m_stopped;
    
public:
    InputSystem* m_pInputSystem;
};

static LRESULT CALLBACK WndProc(HWND,UINT,WPARAM,LPARAM);

static AEngine* appHandle = nullptr;

