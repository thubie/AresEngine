#pragma once
#include"CSLock.h"


template<class Type>
class ConcurrentLLQueue
{
    template <typename T>
    struct Node
    {
        T m_value;
        Node<T>* volatile m_pNext;
    };

public:
    //Construct Queue with dummy node
    ConcurrentLLQueue()
    {
        m_pEnqueueLock = new CSLock();
        m_pDequeueLock = new CSLock();
        m_phead = m_ptail = new Node<Type>();
    }

    //Go through the list and delete all nodes.
    ~ConcurrentLLQueue()
    {
        delete m_pEnqueueLock;
        delete m_pDequeueLock;
        delete m_phead;
        delete m_ptail;
    }

    void emptyQueue()
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
        m_phead = pNext;           
        m_pDequeueLock->LeaveLock(); //Release lock
        return value;           
    }


private:
    Node<Type>* m_phead;
    Node<Type>* m_ptail;
    CSLock* m_pEnqueueLock;
    CSLock* m_pDequeueLock;
};
