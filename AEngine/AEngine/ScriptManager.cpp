#include"ScriptManager.h"
#include"AEngine.h"

extern AEngine* appHandle;

ScriptManager::ScriptManager(AEngine* pEngine, const char* currentDir)
{
    m_pEngine = pEngine;
    m_pLua = lua_open();
    luaL_openlibs(m_pLua);//open lua
    m_pConfigScript = new char[1024];
    m_pRuntimeConfigScript = new char[1024];
    strcpy_s(m_pConfigScript, 1024, currentDir);
    strcpy_s(m_pRuntimeConfigScript, 1024, currentDir);
    strcat_s(m_pConfigScript, 1024, "\\Scripts\\Config.lua");
    strcat_s(m_pRuntimeConfigScript, 1024, "\\Scripts\\RuntimeConfigs.lua");
}

ScriptManager::~ScriptManager()
{
    lua_close(m_pLua); //clean up lua interpreter resources.
    delete[] m_pRuntimeConfigScript;
    delete[] m_pConfigScript;
}

//Reads and set config.
void ScriptManager::GetConfigSetting(unsigned int* widthWindow,unsigned int* heighWindow, unsigned int* numCores)
{
    int iErr = 0;
    char errorstring[1024];
    LUA_ERRFILE;
    if(luaL_loadfile(m_pLua, "Config.lua") || lua_pcall(m_pLua, 0, 0, 0))
    {
        strcpy_s(errorstring, 1024, lua_tostring(m_pLua, -1));
        luaL_error(m_pLua, "Cannot run configfile. file: %s", lua_tostring(m_pLua, -1));
    }
    
    lua_getglobal(m_pLua, "resx"); // stackposition -3   
    lua_getglobal(m_pLua, "resy"); // stackposition-2
    lua_getglobal(m_pLua, "numthread"); // stackposition-1

    if(!lua_isnumber(m_pLua, -3))
        luaL_error(m_pLua, "resx should be a number \n");
    if(!lua_isnumber(m_pLua, -2))
        luaL_error(m_pLua, "resy should be a number \n");
    if(!lua_isnumber(m_pLua, -1))
        luaL_error(m_pLua, "numthread should be a number \n");

    *widthWindow = lua_tointeger(m_pLua, -3);
    *heighWindow = lua_tointeger(m_pLua, -2);
    *numCores = lua_tointeger(m_pLua, -1);

    //clean up lua stack
    int sizeStack = lua_gettop(m_pLua);
    lua_pop(m_pLua, sizeStack);
}

void ScriptManager::SetRuntimeSetting()
{
    Runscript();
}

void ScriptManager::Runscript()
{
    int iErr = 0;
    char errorstring[1024];
    if(luaL_dofile(m_pLua, "RuntimeConfigs.lua"))
    {
        strcpy_s(errorstring, 1024, lua_tostring(m_pLua, -1));
        luaL_error(m_pLua, "Cannot run RuntimeConfigsfile. file: %s", lua_tostring(m_pLua, -1));
    }
    int sizeStack = lua_gettop(m_pLua);
}

//Register the Task system
void ScriptManager::RegisterTaskSystem()
{
    int stacksize = 0;
    stacksize = lua_gettop(m_pLua);
    luaL_newmetatable(m_pLua, "TaskSystem"); //pushes an metatable on the stack 
    lua_pushstring(m_pLua, "__index");
    lua_pushvalue(m_pLua, -2);
    lua_settable(m_pLua, -3);
    luaL_openlib(m_pLua, 0, TaskSystem_methods, 0);
    luaL_openlib(m_pLua, "TaskSystem", TaskSystem_funcs, 0);
}


static int GetTaskSystem(lua_State* lua)
{
    //will push userdata on stack assume as first element if we manage the stack right.
    TaskSystem *ud = static_cast<TaskSystem*>(lua_newuserdata(lua, sizeof(TaskSystem*)));
    TaskSystem* testptr = appHandle->m_pTaskSystem;
    (ud) = appHandle->GetTaskSystem();
    luaL_getmetatable(lua,"TaskSystem");
    lua_setmetatable(lua, -2);
    return 1;
}





