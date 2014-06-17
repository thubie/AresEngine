#include"Runstate.h"

Runstate::Runstate(AEngine* pEngine)
{
    this->pEngine = pEngine;
}

void Runstate::ProcessMessage(MessageQueue* pMessageQueue)
{
    Message currentMessage;
    unsigned int numMessages = pMessageQueue->GetCount();

    for(unsigned int i = 0; i < numMessages; ++i)
    {
        currentMessage = pMessageQueue->Dequeue();
        switch(currentMessage.MessageType)
        {
            case ANIMATIONS_DONE:
            {   
                pEngine->RenderWorld();
                break;
            }

            case RENDERING_DONE:
            {
                pEngine->AnimateWorld();
                break;
            }

            default:
            {
                break;
            }
        }
    }
}