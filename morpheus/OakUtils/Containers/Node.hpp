#ifndef __NODE_HPP__
#define __NODE_HPP__

struct Node
{
    // Everything needs to be null by default otherwise we run into segfaults lol
    const char* Name = nullptr;
    void*       Data = nullptr;
    
    Node*       Prev = nullptr;
    Node*       Next = nullptr;
};

#endif /* !__NODE_HPP__ */