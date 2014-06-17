#pragma once

#include"MessageQueue.h"

class IState
{
    public:
        ~IState(){}; 
        virtual void ProcessMessage(MessageQueue* pMessageQueue) = 0;
};
