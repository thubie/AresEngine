#pragma once

namespace AresEngine
{
    //Templated node for the Queue 
    template <typename T>
    struct Node
    {
        T value;
        node<T> * volatile pNext;
        node() : value(), pNext(nullptr)
        {}
        node(T other) : value(other), pNext(nullptr)
        {}
    };
}