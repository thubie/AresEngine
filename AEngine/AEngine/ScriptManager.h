#pragma once

#include<String.h>
#include<Windows.h>
#include"lua_inc.h"
#include"AEngine.h"
#include"TaskSystem.h"

class AEngine;

int GetTaskSystem(lua_State* lua);

class ScriptManager
{
public:
    ScriptManager(AEngine* pEngine, const char* currentDir);
    ~ScriptManager();
    int LuaOpenLibrary(const luaL_reg* libraryfunctions, const char* libraryName);
    void GetConfigSetting(unsigned int* widthWindow, unsigned int* heighWindow, unsigned int* numCores);
    void SetRuntimeSetting();
private:
    void Runscript();
private:
    lua_State* m_pLuaVM;
    AEngine* m_pEngine;
    char* m_pConfigScript;
    char* m_pRuntimeConfigScript;
};
