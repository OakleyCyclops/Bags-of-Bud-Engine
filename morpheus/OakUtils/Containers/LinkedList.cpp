#include "oakUtilsPCH.hpp"


LinkedList::LinkedList()
{
    head.SetName("head");
    head.SetData(nullptr);
    head.SetPrev(nullptr);
    head.SetNext(nullptr);
}

unsigned int LinkedList::GetSize()
{
    return this->size;
}

Node* LinkedList::GetHead()
{
    return &this->head;
}

Node* LinkedList::Search(const char* name)
{   
    unsigned int i = 0;
    Node* currentNode = GetHead();
    String phrase;

    while (i != GetSize())
    {
        if (!phrase.Icmp(currentNode->GetName(), name))
        {
            return currentNode;
        }
        else
        {
            currentNode = currentNode->GetNext();
        }
        
        i++;
    }

    return nullptr;
}

void LinkedList::Push(Node* node)
{   
    if (size == 0)
    {
        GetHead()->SetName(node->GetName());
        GetHead()->SetData(node->GetData());
        size++;
        return;
    }

    head.SetNext(node);
    node->SetPrev(&head);
    size++;
}

void LinkedList::InsertBefore(Node* nextNode, Node* node)
{
    if (size == 0)
    {
        GetHead()->SetName(node->GetName());
        GetHead()->SetData(node->GetData());
        size++;
        return;
    }

    if (nextNode == nullptr)
    {
        return;
    }

    nextNode->SetPrev(node);
    size++;
}

void LinkedList::InsertAfter(Node* prevNode, Node* node)
{
    if (size == 0)
    {
        GetHead()->SetName(node->GetName());
        GetHead()->SetData(node->GetData());
        size++;
        return;
    }

    if (prevNode == nullptr)
    {
        return;
    }

    if (prevNode->GetNext() != nullptr)
    {
        node->SetNext(prevNode->GetNext());
    }

    prevNode->SetNext(node);
    size++;
}

void LinkedList::Append(Node* node)
{
    Node* currentNode;

    if (size == 0)
    {
        GetHead()->SetName(node->GetName());
        GetHead()->SetData(node->GetData());
        size++;
        return;
    }

    currentNode = GetHead();

    while (currentNode->GetNext() != nullptr)
    {
        currentNode = currentNode->GetNext();
    }

    currentNode->SetNext(node);
    size++;
}