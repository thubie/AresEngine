#include"AEngine.h"
#include<stdio.h>
#include<iostream>


void MyCreateThreads(HANDLE* threadArray, DWORD* threadIdArray, unsigned short numProcs,FILE* file);
DWORD WINAPI LogThreads(LPVOID lpParameter);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pSCmdLine,int iCmdShow)
{
    bool result;
    AEngine* pAEngine;

    //Create the app object
    pAEngine = new AEngine();

    if(!pAEngine)
        return 0;

    //Intialize and check it then run
    result = pAEngine->Initialize();

    if(result)
    {
        pAEngine->Run();
    }


    return 0;
}

