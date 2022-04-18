#include "oakUtilsPCH.hpp"


LinkedList::LinkedList()
{
    head.SetName("head");
    head.SetData(nullptr);
}

unsigned int LinkedList::GetSize() const
{
    return size;
}

Node* LinkedList::GetHead()
{
    return &head;
}

Node* LinkedList::Search(const char* name) const
{
    unsigned int i;
    Node* currentNode;

    currentNode = head.GetNext();

    while (i != size || currentNode->GetName() != name)
    {
        currentNode = currentNode->GetNext();
        i++;
    }

    if (i == size && currentNode->GetName() != name)
    {
        return NULL;
    }

    else
    {
        return currentNode;
    }

}

void LinkedList::Push(Node* node)
{
    head.SetNext(node);
    node->SetPrev(&head);

    size++;
}

void LinkedList::Append(Node* node)
{
    
}