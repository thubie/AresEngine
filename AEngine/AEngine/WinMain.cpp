#include"AEngine.h"

class AEngine;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pSCmdLine,int iCmdShow)
{
    bool result;
    
    //Create the app object
    AEngine* engine;
    engine = new AEngine();

    if(!engine)
        return 0;

    //Intialize and check it then run
    result = engine->Initialize();

    if(result)
    {
        engine->Run();
    }

    return 0;
}



