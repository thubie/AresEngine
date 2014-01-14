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

class InputSystem;
class GeometryManager;
class RenderSystem;

class AEngine
{
public:
    AEngine();
    AEngine(const AEngine& other);
    ~AEngine();

    bool Initialize();
    void Run();
    bool Shutdown();

    //Test input code delete later.
    void MoveCameraForward();
    void MoveCameraBackward();

private:
    bool Draw();
    void InitializeWin();

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
    int m_windowWidth; //To do read window resolution from a config/ini file
    int m_windowHeight;
    bool m_stopped;
    float ElapsedGameTime; //Total time running.

public:
    InputSystem* m_pInputSystem;
};

static LRESULT CALLBACK WndProc(HWND,UINT,WPARAM,LPARAM);

static AEngine* appHandle = nullptr;

