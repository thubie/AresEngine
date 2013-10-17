#include"AEngine.h"
#include<stdio.h>
#include<iostream>
#include"TaskManager.h"

void MyCreateThreads(HANDLE* threadArray, DWORD* threadIdArray, unsigned short numProcs,FILE* file);
DWORD WINAPI LogThreads(LPVOID lpParameter);
using namespace AresEngine;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pSCmdLine,int iCmdShow)
{
    bool result;
    AEngine* pAEngine;
    TaskManager* pTaskManager;

    //Create the app object
    pAEngine = new AEngine();
    pTaskManager = new TaskManager();


    if(!pAEngine)
        return 0;

    //Intialize and check it then run
    result = pAEngine->Initialize();
    pTaskManager->Initialize();



    if(result)
    {
        pAEngine->Run();
    }

    pTaskManager->Shutdown();

    return 0;
}

