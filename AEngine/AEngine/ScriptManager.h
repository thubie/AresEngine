#pragma once

#include<String.h>
#include<Windows.h>
#include"lua_inc.h"
#include"TaskSystem.h"
#include"RenderSystem.h"


//class TaskSystem;

class ScriptManager
{
public:
    ScriptManager(const char* currentDir);
    ~ScriptManager();
    void GetConfigSetting(unsigned int* widthWindow, unsigned int* heighWindow, unsigned int* numCores);
    void SetRuntimeSetting();
    void RegisterTaskSystem(TaskSystem* pTaskSystem);
private:
    void Runscript();
    static int GetTaskSystem(lua_State* lua);
private:
    lua_State* m_pLua;
    TaskSystem* m_pTaskSystem;
    char* m_pConfigScript;
    char* m_pRuntimeConfigScript;
};

