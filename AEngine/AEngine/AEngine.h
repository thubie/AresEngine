#pragma once

#include<Windows.h>
#include"TaskManager.h"
#include"CSLock.h"
#include"CounterTask.h"
#include"Model.h"
#include"RenderSystem.h"
#include"HRTimer.h"
#define WIN32_LEAN_AND_MEAN


class AEngine
{
public:
    AEngine();
    AEngine(const AEngine& other);
    ~AEngine();

    bool Initialize();
    void Run();
    bool Shutdown();

private:
    bool Draw();
    void InitializeWin();

private:
    LPCWSTR         m_appName;
    HINSTANCE       m_hInstance;
    HWND            m_hwnd;
    TaskManager*    m_pTaskManager;
    CounterTask*    m_TestTask;
    RenderSystem*   m_pRenderSystem ;
    HRTimer*        m_pGameTimer;
    Model*          m_pTestRenderModel;

    int             m_windowWidth; //To do read window resolution from a config/ini file
    int             m_windowHeight;

};

static LRESULT CALLBACK WndProc(HWND,UINT,WPARAM,LPARAM);

static AEngine* appHandle = nullptr;

