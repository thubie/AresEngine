#include "InitializeState.h"

InitializeState::InitializeState(AEngine* pEngine)
{
    this->pEngine = pEngine;
    importingBits = 0;
}

void InitializeState::ProcessMessage(MessageQueue* pMessageQueue)
{
    Message currentMessage;
    unsigned int numMessages = pMessageQueue->GetCount();

    for(unsigned int i = 0; i < numMessages; ++i)
    {
        currentMessage = pMessageQueue->Dequeue();
        switch(currentMessage.MessageType)
        {
            case  IMPORT_TEXTURE_DONE:
            {
                importingBits += IMPORT_TEXTURE_DONE;
                if(importingBits == IMPORTING_DONE)
                {
                    Message message;
                    message.MessageType = IMPORTING_DONE;
                    pMessageQueue->Enqueue(message);
                }
                break;
            }

            case IMPORT_GEOMETRY_DONE:
            {
                importingBits += IMPORT_GEOMETRY_DONE;
                if(importingBits == IMPORTING_DONE)
                {
                    Message message;
                    message.MessageType = IMPORTING_DONE;
                    pMessageQueue->Enqueue(message);
                }
                break;
            }

            case IMPORT_VERTEXSHADER_DONE:
            {
                importingBits += IMPORT_VERTEXSHADER_DONE;
                if(importingBits == IMPORTING_DONE)
                {
                    Message message;
                    message.MessageType = IMPORTING_DONE;
                    pMessageQueue->Enqueue(message);
                }
                break;
            }

            case IMPORT_PIXELSHADER_DONE:
            {
                importingBits += IMPORT_PIXELSHADER_DONE;
                if(importingBits == IMPORTING_DONE)
                {
                    Message message;
                    message.MessageType = IMPORTING_DONE;
                    pMessageQueue->Enqueue(message);
                }
                break;
            }

            case IMPORT_ANIMATION_DONE:
            {
                importingBits += IMPORT_ANIMATION_DONE;
                if(importingBits == IMPORTING_DONE)
                {
                    Message message;
                    message.MessageType = IMPORTING_DONE;
                    pMessageQueue->Enqueue(message);
                }
                break;
            }

            case IMPORTING_DONE:
            {
                //Set Engine to Runstate
                pEngine->ChangeToRunstate();
                break;
            }

            default:
            {
                break;
            }
        }
    }
}