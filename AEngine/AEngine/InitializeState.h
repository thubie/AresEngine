#pragma once

#include "IState.h"
#include "AEngine.h"

enum InitializeMessageType
{
    IMPORT_ANIMATION_DONE = 1,
    IMPORT_TEXTURE_DONE = 2,
    IMPORT_GEOMETRY_DONE = 4,
    IMPORT_VERTEXSHADER_DONE = 8,
    IMPORT_PIXELSHADER_DONE = 16,
    IMPORTING_DONE = 31,
};

class InitializeState : public IState
{
public:
    InitializeState(AEngine* pEngine);
    void ProcessMessage(MessageQueue* pMessageQueue);
private:
    AEngine* pEngine;
    unsigned int importingBits;
};