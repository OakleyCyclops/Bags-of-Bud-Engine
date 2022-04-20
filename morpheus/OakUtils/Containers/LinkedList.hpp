#ifndef __LINKED_LIST_HPP__
#define __LINKED_LIST_HPP__

class LinkedList
{
    public:
        friend class Node;
        LinkedList();
        
        Node*           Search(const char* name);
        void            Push(Node* node);
        void            Append(Node* node);
        void            InsertBefore(Node* nextNode, Node* node);
        void            InsertAfter(Node* prevNode, Node* node);

        void            Clear();

        // Getter functions
        unsigned int    GetSize();
        Node*           GetHead();  // lol

        bool            IsEmpty();
    
    private:
        Node             head;
        unsigned int     size;
};

#endif /* !__LINKED_LIST_HPP__ */