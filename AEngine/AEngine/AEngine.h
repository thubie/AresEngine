#pragma once

#include<Windows.h>
#include"CSLock.h"

#define WIN32_LEAN_AND_MEAN

namespace AresEngine
{
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
        LPCWSTR m_appName;
        HINSTANCE m_hInstance;
        HWND m_hwnd;
        CSLock* mp_lock;
        

        //To do read window resolution from a config/ini file
        int m_windowWidth;
        int m_windowHeight; 
    };

    static LRESULT CALLBACK WndProc(HWND,UINT,WPARAM,LPARAM);

    static AEngine* appHandle = nullptr;

}
