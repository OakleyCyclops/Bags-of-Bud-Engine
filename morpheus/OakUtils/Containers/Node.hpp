#ifndef __NODE_HPP__
#define __NODE_HPP__

class Node
{
    friend class LinkedList;
    public:
        Node()
        {
            assert(typeid(this) != typeid(Node));
        }

        Node(const char* Name, void* Data);

        const char* GetName() const;
        void* GetData() const;

        Node* GetPrev() const;
        Node* GetNext() const;

    protected:
        // There isn't really a reason to use these outside of making the class work
        void SetName(const char* name);
        void SetData(void* data);
        void SetPrev(Node* prevNode);
        void SetNext(Node* nextNode);

    private:
        const char* Name;
        void*       Data;

        Node*       Prev;
        Node*       Next;
};

#endif /* !__NODE_HPP__ */