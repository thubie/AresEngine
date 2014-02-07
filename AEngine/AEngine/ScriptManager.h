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
    void GetConfigSetting(unsigned int* widthWindow, unsigned int* heighWindow, unsigned int* numCores);
    void SetRuntimeSetting();
    void RegisterTaskSystem();
private:
    void Runscript();
    static int GetTaskSystem(lua_State* lua);
private:
    lua_State* m_pLua;
    AEngine* m_pEngine;
    char* m_pConfigScript;
    char* m_pRuntimeConfigScript;
};

static const luaL_reg TaskSystem_funcs[] =
{
    {"GetTaskSystem", GetTaskSystem},
    {NULL, NULL}
};

static const luaL_reg TaskSystem_methods[] = 
{
    {"SetNUmWorkers", SetNumWorkers},
    {NULL, NULL}
};
