#pragma once
#include"Lock.h"

//Multiple consumer and producer linked list

template<class Type>
class LinkListQueue
{
    template <typename T>
    struct Node
    {
        T m_value;
        Node<T>* volatile m_pNext;
    };

public:
    //Construct Queue with dummy node
    LinkListQueue()
    {
        m_pEnqueueLock = new Lock;
        m_pDequeueLock = new Lock;
        m_phead = m_ptail = new Node<Type>();
         m_nodeCount = 0;
    }

    //Go through the list and delete all nodes.
    ~LinkListQueue()
    {
        delete m_pEnqueueLock;
        delete m_pDequeueLock;
        delete m_phead;
        delete m_ptail;
    }

    void EmptyQueue()
    {
        Node<Type>* current = m_phead;
        Node<Type>* next;
        while(current != m_ptail)
        {
            next = current->m_pNext;
            delete current;
            current = next;
        }
        m_phead = nullptr;
    }

    //Enqueue a new node onto the queue 
    void Enqueue(Type object)
    {
        Node<Type>* newNode = new Node<Type>();
        newNode->m_value = object;
        m_pEnqueueLock->EnterLock();
        m_ptail->m_pNext = newNode;
        m_ptail = newNode;
        m_nodeCount ++;
        m_pEnqueueLock->LeaveLock();
    }

    //Dequeue and return a new node.
    Type Dequeue()
    {
        Type value;
        Node<Type>* pNext;
        m_pDequeueLock->EnterLock(); //Enter lock
        pNext = nullptr;
        pNext = m_phead->m_pNext;
        if(pNext == nullptr)
        {
            m_pDequeueLock->LeaveLock(); //Leave lock if pNext is nullptr.
            return nullptr;
        }
        value  = pNext->m_value;
        delete m_phead;
        m_phead = pNext;
        m_nodeCount--;
        m_pDequeueLock->LeaveLock(); //Release lock
        return value;           
    }

    inline unsigned int GetCount()
    {
        return m_nodeCount;
    }

    inline bool QueueIsEmpty()
    {
        return m_nodeCount == 0;
    }

private:
    Node<Type>* m_phead;
    Node<Type>* m_ptail;
    Lock* m_pEnqueueLock;
    Lock* m_pDequeueLock;
    unsigned int m_nodeCount;
};
