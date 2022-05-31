#include "oakUtilsPCH.hpp"

Node* linkedList::Search(LinkedList* list, const char* name)
{   
    unsigned int i = 0;
    Node* currentNode = &list->Head;
    std::string phrase(name);

    while (i != list->Size)
    {
        if (!phrase.compare(currentNode->Name))
        {
            return currentNode;
        }
        else
        {
            currentNode = currentNode->Next;
        }
        
        i++;
    }

    return nullptr;
}

void linkedList::Push(LinkedList* list, Node* node)
{   
    if (list->Size == 0)
    {
        list->Head.Name = node->Name;
        list->Head.Data = node->Data;

        list->Size++;
        return;
    }

    list->Head.Next = node;
    node->Prev = &list->Head;
    list->Size++;
}

void linkedList::InsertBefore(LinkedList* list, Node* nextNode, Node* node)
{
    if (list->Size == 0)
    {
        list->Head.Name = node->Name;
        list->Head.Data = node->Data;

        list->Size++;
        return;
    }

    if (nextNode == nullptr)
    {
        return;
    }

    nextNode->Prev = node;
    list->Size++;
}

void linkedList::InsertAfter(LinkedList* list, Node* prevNode, Node* node)
{
    if (list->Size == 0)
    {
        list->Head.Name = node->Name;
        list->Head.Data = node->Data;

        list->Size++;
        return;
    }

    if (prevNode == nullptr)
    {
        return;
    }

    prevNode->Next = node;
    list->Size++;
}

void linkedList::Append(LinkedList* list, Node* node)
{
    Node* currentNode;

    if (list->Size == 0)
    {
        list->Head.Name = node->Name;
        list->Head.Data = node->Data;

        list->Size++;
        return;
    }

    currentNode = &list->Head;

    while (currentNode->Next != nullptr)
    {
        currentNode = currentNode->Next;
    }

    currentNode->Next = node;
    list->Size++;
}