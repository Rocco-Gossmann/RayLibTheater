#ifndef STAGE_CORE_DOUBLE_LINKED_LIST_H 
#define STAGE_CORE_DOUBLE_LINKED_LIST_H 1


namespace Stage {
namespace Core {


template <typename T> class DoubleLinkedList {
public:

    DoubleLinkedList<T>(): _next(0), _prev(0) { 
        _head = this; 
        _tail = this; 
    };

    /**
     * @brief Add a new Item to list, after this Node
     *
     * @param item 
     * @return - the node in the list, that was created
     */
    DoubleLinkedList<T> Append(T* item) {
        if(item == 0) throw "cant add NULL / 0 / nullptr to a linked list";

        auto node = newNode();
        node->_item = item;

        // Set new tail, when appending to the last element of the list
        if (this->_next == 0) 
            this->_head->_tail = node;

        // Connect the new node to the list
        node->_prev = this;
        node->_next = this->_next;
        this->_next = node;

        return node;
    }

    /**
     * @brief Add a new Item to list, before this Node
     *
     * If this node is the root of a list, the item is added after this node
     *
     * @param item 
     * @return - the node in the list, that was created
     */
    DoubleLinkedList<T>* Prepend(T* item) {
        if(item == 0) throw "cant add NULL / 0 / nullptr to a linked list";

        auto node = newNode();
        node->_item = item;

        // Set new tail, when appending to the last element of the list
        if (this->_prev == 0)  {
            // Cant prepend the the _head of a list, append to the head instead (chaning the first elment of the list)
            return this->Append(item);
        }

        node->_next = this;
        node->_prev = this->_prev;
        this->_prev = node;

        return node;
    }

    /**
     * @brief finds the node of an item after this node.
     *
     * @param item - 0 if the item could not be found
     */
    DoubleLinkedList<T>* Find(T* item) {

        auto node = this->_head;
        while ((node = node->Next()) != 0) {
            if (node->_item == item) 
                return node;
        }

        return 0;
         
    }

    DoubleLinkedList<T>* Next() {
        return _next;
    }


    void Drop() {
        if(_tail != 0) throw "can't drop the root of a DoubleLinkedList";

        if(_prev != 0) _prev->_next = this->_next;
        if(_next != 0) _next->_prev = this->_prev;

        delete(this);
    };

protected:
    T* _item;
    DoubleLinkedList<T>* _next;
    DoubleLinkedList<T>* _prev;
    DoubleLinkedList<T>* _head;
    DoubleLinkedList<T>* _tail;

private:
    DoubleLinkedList<T>* newNode() {
        auto node = new DoubleLinkedList<T>();
        node->_head = this->_head;
        node->_tail = 0; // Only the head can know the Tail
        return node;
    }
};






}
}

#endif // STAGE_CORE_DOUBLE_LINKED_LIST_H
