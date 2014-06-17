#pragma once

#include"IState.h"
#include"AEngine.h"

enum RunMessageType
{
    ANIMATIONS_DONE = 200,
    RENDERING_DONE = 201,
};

class Runstate : public IState
{
public:
    Runstate(AEngine* pEngine);
    void ProcessMessage(MessageQueue* pMessageQueue);

private:
    AEngine* pEngine;
};