#pragma once
#include"Lock.h"
#include"Message.h"

class MessageQueue
{
public:
    MessageQueue(unsigned int maxSize);
    ~MessageQueue();

    void Enqueue(Message message);
    Message Dequeue();
    unsigned int GetCount();

private:
    Message* m_pMessageQueue;
    unsigned int m_Head;
    unsigned int m_Tail;
    unsigned int m_QueueLength;
    unsigned int m_Count;
    Lock* m_pEnqueueLock;
    Lock* m_pDequeueLock;
};