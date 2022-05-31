#ifndef __LINKED_LIST_HPP__
#define __LINKED_LIST_HPP__

struct LinkedList
{
        Node             Head;
        unsigned int     Size;
};

namespace linkedList
{
    Node*           Search(LinkedList* list, const char* name);
    void            Push(LinkedList* list, Node* node);
    void            Append(LinkedList* list, Node* node);
    void            InsertBefore(LinkedList* list, Node* nextNode, Node* node);
    void            InsertAfter(LinkedList* list, Node* prevNode, Node* node);

    void            Clear();
    bool            IsEmpty();
};

#endif /* !__LINKED_LIST_HPP__ */