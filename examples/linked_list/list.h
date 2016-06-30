#ifndef __LIST_H__
#define __LIST_H__

struct ListNode
{
    std::string str;
    int x;
    ListNode* next;
};

class LinkedList {
  public:
    LinkedList()
    {
        this->head = nullptr;
        this->tail = nullptr;
    }

    void AddToHead(std::string str, int x)
    {
        ListNode * node = new ListNode;
        node->str = str;
        node->x = x;
        node->next = this->head;
        this->head = node;
        if (this->tail == nullptr)
            this->tail = node;
    }

    void AddToTail(std::string str, int x)
    {
        ListNode * node = new ListNode;
        node->str = str;
        node->x = x;
        node->next = nullptr;
        if (this->tail == nullptr)
        {
            this->tail = node;
            this->head = node;
        }
        else
        {
            this->tail->next = node;
            this->tail = node;
        }
    }

    ListNode * head;
    ListNode * tail;
};


#endif
