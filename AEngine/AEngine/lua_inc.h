#pragma once


//Need to use extern "C" because the libs are compiled in C
extern "C"
{
#include<lua.h>
#include<lualib.h>
#include<lauxlib.h>
}