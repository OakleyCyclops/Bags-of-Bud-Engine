#include "oakUtilsPCH.hpp"

Node::Node(const char* Name, void* Data)
{
    this->Name = Name;
    this->Data = Data;
    this->Next = nullptr;
    this->Prev = nullptr;
}

const char* Node::GetName() const
{
    return Name;
}

void* Node::GetData() const
{
    return Data;
}

Node* Node::GetPrev() const 
{
    return Prev;
}

Node* Node::GetNext() const
{
    return Next;
}

void Node::SetName(const char* name)
{
    this->Name = name;
}

void Node::SetData(void* data)
{
    this->Data = data;
}

void Node::SetPrev(Node* prevNode)
{
    this->Prev = prevNode;
}

void Node::SetNext(Node* nextNode)
{
    this->Next = nextNode;
}