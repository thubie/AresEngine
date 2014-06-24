#include"ScriptManager.h"
#include"AEngine.h"

extern AEngine* appHandle;

ScriptManager::ScriptManager(AEngine* pEngine, const char* currentDir)
{
    m_pEngine = pEngine;
    m_pLuaVM = lua_open();
    luaL_openlibs(m_pLuaVM);//open lua
    m_pConfigScript = new char[1024];
    m_pRuntimeConfigScript = new char[1024];
    strcpy_s(m_pConfigScript, 1024, currentDir);
    strcpy_s(m_pRuntimeConfigScript, 1024, currentDir);
    strcat_s(m_pConfigScript, 1024, "\\Scripts\\Config.lua");
    strcat_s(m_pRuntimeConfigScript, 1024, "\\Scripts\\RuntimeConfigs.lua");
}

ScriptManager::~ScriptManager()
{
    lua_close(m_pLuaVM); //clean up lua interpreter resources.
    delete[] m_pRuntimeConfigScript;
    delete[] m_pConfigScript;
}

//Reads and set config.
void ScriptManager::GetConfigSetting(unsigned int* widthWindow,unsigned int* heighWindow, unsigned int* numCores)
{
    int iErr = 0;
    char errorstring[1024];
    LUA_ERRFILE;
    size_t convertedChar = 0;
    wchar_t debugString[1024];

    lua_pushlightuserdata(m_pLuaVM, (void*)m_pEngine);
    lua_setglobal(m_pLuaVM, "world");

    if(luaL_loadfile(m_pLuaVM, m_pConfigScript) || lua_pcall(m_pLuaVM, 0, 0, 0))
    {
        strcpy_s(errorstring, 1024, lua_tostring(m_pLuaVM, -1));
        luaL_error(m_pLuaVM, "Cannot run configfile. file: %s", lua_tostring(m_pLuaVM, -1));
        mbstowcs_s(&convertedChar,debugString,errorstring,1024);
        OutputDebugString(debugString);
    }    
    lua_getglobal(m_pLuaVM, "resx"); // stackposition -3   
    lua_getglobal(m_pLuaVM, "resy"); // stackposition-2
    lua_getglobal(m_pLuaVM, "numthread"); // stackposition-1

    if(!lua_isnumber(m_pLuaVM, -3))
        luaL_error(m_pLuaVM, "resx should be a number \n");
    if(!lua_isnumber(m_pLuaVM, -2))
        luaL_error(m_pLuaVM, "resy should be a number \n");
    if(!lua_isnumber(m_pLuaVM, -1))
        luaL_error(m_pLuaVM, "numthread should be a number \n");

    *widthWindow = lua_tointeger(m_pLuaVM, -3);
    *heighWindow = lua_tointeger(m_pLuaVM, -2);
    *numCores = lua_tointeger(m_pLuaVM, -1);

    //clean up lua stack
    int sizeStack = lua_gettop(m_pLuaVM);
    lua_pop(m_pLuaVM, sizeStack);
}

int ScriptManager::LuaOpenLibrary(const luaL_reg* libraryfunctions, const char* libraryName)
{
    luaL_openlib(m_pLuaVM, libraryName, libraryfunctions, 0);
    return 1;
}

void ScriptManager::SetRuntimeSetting()
{
    Runscript();
}

void ScriptManager::Runscript()
{
    int iErr = 0;
    char errorstring[1024];
    size_t convertedChar = 0;
    wchar_t debugString[1024];
    LUA_ERRFILE;
    
    if(luaL_loadfile(m_pLuaVM, m_pRuntimeConfigScript) || lua_pcall(m_pLuaVM, 0, 0, 0))
    {
        strcpy_s(errorstring, 1024, lua_tostring(m_pLuaVM, -1));
        luaL_error(m_pLuaVM, "Cannot run configfile. file: %s", lua_tostring(m_pLuaVM, -1));
        mbstowcs_s(&convertedChar,debugString,errorstring,1024);
        OutputDebugString(debugString);
    }    
    
}










