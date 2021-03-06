#ifndef SJTU_DEQUE_HPP
#define SJTU_DEQUE_HPP

#include "exceptions.hpp"

#include <cstddef>

namespace sjtu {


    template<class T>
    class deque {
    public:
        const int nodeLength = 256;  //set the max size of each node
        int length; //store the number of elements in dequeue

        //data module
        struct node {
            node *prev, *next;  //pointers, pointing to the previous and next node
            T **data;   //class T pointer array
            int nodeSize;   //the number of elements in this node
            //construction
            node(int s, node *p = NULL, node *n = NULL) {
                //initialize
                data = new T *[s];
                nodeSize = 0;
                prev = p;
                next = n;
            }

            //destruction
            ~node() {
                if (this != NULL) {
                    if (data) {
                        for (int i = 0; i < nodeSize; i++)
                            if (data[i]) {
                                delete data[i];
                                data[i] = NULL;
                            }
                    }
                    if (data) delete[]data;
                    data = NULL;
                    prev = next = NULL;
                    nodeSize = 0;
                }
            }
        };

        node *head, *tail;  //pointers, pointing to the head-node and tail-node

        //clear all of the nodes
        void clearAll() {
            if (head == NULL) return;
            node *tmp = head;
            node *del;
            while (tmp) {
                del = tmp;
                tmp = tmp->next;
                delete del;
            }
            head = tail = new node(nodeLength, NULL, NULL);
            length = 0;
        }

        //search for the NO.rank+1 elements(whose subscript index is rank as well)
        void search(const int rank, node *&pos, int &nodePos) {
            //if the rank exceeds  the scope of dequeue
            if (rank >= length) {
                pos = NULL;
                nodePos = -1;
                return;
            }
            node *tmp = head;
            unsigned long long counter = rank + 1; //set a counter
            //to find the element in which node
            while (tmp->nodeSize < counter) {
                counter -= tmp->nodeSize;
                tmp = tmp->next;
            }
            pos = tmp;
            nodePos = counter - 1;
        }



    public:
        class const_iterator;

        class iterator {
            friend class deque<T>;

            friend class const_iterator;

        private:
            deque<T> *que;
            node *currentNode;  //pointer, pointing to the current node
            int nodePos;    //store the position in the node, starting from 0

        public:
            //construction
            iterator() : que(NULL), currentNode(NULL), nodePos(-1) {}

            iterator(deque<T> *q, node *cn, int np) {
                que = q;
                currentNode = cn;
                nodePos = np;
            }

            iterator(const iterator &rhs) {
                que = rhs.que;
                currentNode = rhs.currentNode;
                nodePos = rhs.nodePos;
            }


            //overload operator =
            iterator &operator=(const iterator &other) {
                que = other.que;
                currentNode = other.currentNode;
                nodePos = other.nodePos;
                return *this;
            }

            //return a new iterator which pointer n-next elements
            //even if there are not enough elements, throw invaild_iterator().
            iterator operator+(const int &n) const {
                if (currentNode == NULL) throw invalid_iterator();
                if (n < 0) return (*this) - (-n);
                iterator tmp(*this);
                int counter = n;
                while (counter >= tmp.currentNode->nodeSize - tmp.nodePos) {
                    if (tmp.currentNode->next == NULL) break;

                    counter -= (tmp.currentNode->nodeSize - tmp.nodePos);
                    //to the next block
                    tmp.nodePos = 0;
                    tmp.currentNode = tmp.currentNode->next;
                }
                tmp.nodePos += counter;
                if (tmp.nodePos >= tmp.currentNode->nodeSize) {
                    if (tmp.currentNode->next == NULL
                        && tmp.nodePos == tmp.currentNode->nodeSize)
                        return tmp;
                    tmp.currentNode = NULL;
                    tmp.nodePos = -1;
                }
                return tmp;
            }

            //return a new iterator which pointer n-previous elements
            //even if there are not enough elements, throw invaild_iterator().
            iterator operator-(const int &n) const {
                if (currentNode == NULL) throw invalid_iterator();
                if (n < 0) return (*this) + (-n);
                iterator tmp(*this);
                int counter = n;
                while (counter > tmp.nodePos) {
                    if (tmp.currentNode->prev == NULL) break;

                    counter -= tmp.nodePos + 1;
                    //to the previous block
                    tmp.nodePos = tmp.currentNode->prev->nodeSize - 1;
                    tmp.currentNode = tmp.currentNode->prev;
                }
                tmp.nodePos -= counter;
                if (tmp.nodePos < 0) {
                    tmp.currentNode = NULL;
                    tmp.nodePos = -1;
                }
                return tmp;
            }

            // return the distance between two iterator,
            // if these two iterators points to different vectors, throw invaild_iterator.
            int operator-(const iterator &rhs) const {
                if (que != rhs.que) throw invalid_iterator();

                int counter = 0;
                node *p = currentNode, *q = rhs.currentNode;
                if (p == q) return nodePos - rhs.nodePos;

                //if p is before q
                while (p != NULL && q != NULL) {
                    if (p == q) break;
                    counter += p->nodeSize;
                    p = p->next;
                }
                if (p != NULL) return -(counter + rhs.nodePos - nodePos);

                //if p is after q
                p = currentNode, q = rhs.currentNode;
                counter = 0;
                while (p != NULL && q != NULL) {
                    if (p == q) break;
                    counter += q->nodeSize;
                    q = q->next;
                }
                return counter - rhs.nodePos + nodePos;

            }

            iterator operator+=(const int &n) {
                (*this) = (*this) + n;
                return *this;
            }

            iterator operator-=(const int &n) {
                *this = (*this) - n;
                return *this;
            }

            //iterator++
            iterator operator++(int) {
                iterator tmp(*this);
                (*this) += 1;
                return tmp;
            }

            //++iterator
            iterator &operator++() {
                (*this) += 1;
                return *this;
            }

            iterator operator--(int) {
                iterator tmp(*this);
                (*this) -= 1;
                return tmp;
            }

            iterator &operator--() {
                (*this) -= 1;
                return *this;
            }

            //*iter->field
            T &operator*() const {
                if (que == NULL) throw invalid_iterator();
                if (currentNode == NULL) throw invalid_iterator();
                if (nodePos >= currentNode->nodeSize
                    || nodePos < 0)
                    throw invalid_iterator();

                return *(currentNode->data[nodePos]);
            }

            //iter->field
            T *operator->() const noexcept {
                if (que == NULL) throw invalid_iterator();
                if (currentNode == NULL) throw invalid_iterator();
                if (nodePos >= currentNode->nodeSize
                    || nodePos < 0)
                    throw invalid_iterator();
                return (currentNode->data[nodePos]);
            }

            //a operator to check whether two iterators are same
            // (pointing to the same memory).
            bool operator==(const iterator &rhs) const {
                return que == rhs.que
                       && currentNode == rhs.currentNode
                       && nodePos == rhs.nodePos;
            }

            bool operator==(const const_iterator &rhs) const {
                return que == rhs.que
                       && currentNode == rhs.currentNode
                       && nodePos == rhs.nodePos;
            }

            //some other operator for iterator.
            bool operator!=(const iterator &rhs) const {
                return !(*this == rhs);
            }

            bool operator!=(const const_iterator &rhs) const {
                return !(*this == rhs);
            }
        };

        class const_iterator {
            friend class iterator;

            friend class deque<T>;

        private:
            const deque<T> *que;
            const node *currentNode;  //pointer, pointing to the current node
            int nodePos;    //store the position in the node, starting from 0

        public:
            //construction
            const_iterator() : que(NULL), currentNode(NULL), nodePos(-1) {}

            const_iterator(const deque<T> *q, const node *cn, int np) {
                que = q;
                currentNode = cn;
                nodePos = np;
            }

            const_iterator(const iterator &rhs) {
                que = rhs.que;
                currentNode = rhs.currentNode;
                nodePos = rhs.nodePos;
            }

            const_iterator(const const_iterator &rhs) {
                que = rhs.que;
                currentNode = rhs.currentNode;
                nodePos = rhs.nodePos;
            }


            //overload operator =
            const_iterator &operator=(const const_iterator &other) {
                que = other.que;
                currentNode = other.currentNode;
                nodePos = other.nodePos;
                return *this;
            }

            //return a new iterator which pointer n-next elements
            //even if there are not enough elements, throw invaild_iterator().
            const_iterator operator+(const int &n) const {
                if (currentNode == NULL) throw invalid_iterator();
                if (n < 0) return (*this) - (-n);
                const_iterator tmp(*this);
                int counter = n;
                while (counter >= tmp.currentNode->nodeSize - tmp.nodePos) {
                    if (tmp.currentNode->next == NULL) break;

                    counter -= (tmp.currentNode->nodeSize - tmp.nodePos);
                    //to the next block
                    tmp.nodePos = 0;
                    tmp.currentNode = tmp.currentNode->next;
                }
                tmp.nodePos += counter;
                if (tmp.nodePos >= tmp.currentNode->nodeSize) {
                    if (tmp.currentNode->next == NULL
                        && tmp.nodePos == tmp.currentNode->nodeSize)
                        return tmp;
                    tmp.currentNode = NULL;
                    tmp.nodePos = -1;
                }
                return tmp;
            }

            //return a new iterator which pointer n-previous elements
            //even if there are not enough elements, throw invaild_iterator().
            const_iterator operator-(const int &n) const {
                if (currentNode == NULL) throw invalid_iterator();
                if (n < 0) return (*this) + (-n);
                const_iterator tmp(*this);
                int counter = n;
                while (counter > tmp.nodePos) {
                    if (tmp.currentNode->prev == NULL) break;

                    counter -= tmp.nodePos + 1;
                    //to the previous block
                    tmp.nodePos = tmp.currentNode->prev->nodeSize - 1;
                    tmp.currentNode = tmp.currentNode->prev;
                }
                tmp.nodePos -= counter;
                if (tmp.nodePos < 0) {
                    tmp.currentNode = NULL;
                    tmp.nodePos = -1;
                }
                return tmp;
            }

            // return the distance between two iterator,
            // if these two iterators points to different vectors, throw invaild_iterator.
            int operator-(const const_iterator &rhs) const {
                if (que != rhs.que) throw invalid_iterator();

                int counter = 0;
                const node *p = currentNode, *q = rhs.currentNode;
                if (p == q) return nodePos - rhs.nodePos;

                //if p is before q
                while (p != NULL && q != NULL) {
                    if (p == q) break;
                    counter += p->nodeSize;
                    p = p->next;
                }
                if (p != NULL) return -(counter + rhs.nodePos - nodePos);

                //if p is after q
                p = currentNode, q = rhs.currentNode;
                counter = 0;
                while (p != NULL && q != NULL) {
                    if (p == q) break;
                    counter += q->nodeSize;
                    q = q->next;
                }
                return counter - rhs.nodePos + nodePos;

            }

            const_iterator operator+=(const int &n) {
                (*this) = (*this) + n;
                return *this;
            }

            const_iterator operator-=(const int &n) {
                *this = (*this) - n;
                return *this;
            }

            //iterator++
            const_iterator operator++(int) {
                const_iterator tmp(*this);
                (*this) += 1;
                return tmp;
            }

            //++iterator
            const_iterator &operator++() {
                (*this) += 1;
                return *this;
            }

            const_iterator operator--(int) {
                const_iterator tmp(*this);
                (*this) -= 1;
                return tmp;
            }

            const_iterator &operator--() {
                (*this) -= 1;
                return *this;
            }

            //*iter->field
            T &operator*() const {
                if (que == NULL) throw invalid_iterator();
                if (currentNode == NULL) throw invalid_iterator();
                if (nodePos >= currentNode->nodeSize
                    || nodePos < 0)
                    throw invalid_iterator();

                return *(currentNode->data[nodePos]);
            }

            //iter->field
            T *operator->() const noexcept {
                if (que == NULL) throw invalid_iterator();
                if (currentNode == NULL) throw invalid_iterator();
                if (nodePos >= currentNode->nodeSize
                    || nodePos < 0)
                    throw invalid_iterator();
                return (currentNode->data[nodePos]);
            }

            //a operator to check whether two iterators are same
            // (pointing to the same memory).
            bool operator==(const iterator &rhs) const {
                return que == rhs.que
                       && currentNode == rhs.currentNode
                       && nodePos == rhs.nodePos;
            }

            bool operator==(const const_iterator &rhs) const {
                return que == rhs.que
                       && currentNode == rhs.currentNode
                       && nodePos == rhs.nodePos;
            }

            //some other operator for iterator.
            bool operator!=(const iterator &rhs) const {
                return !(*this == rhs);
            }

            bool operator!=(const const_iterator &rhs) const {
                return !(*this == rhs);
            }

        };

        //construction
        deque() {
            head = tail = new node(nodeLength, NULL, NULL);
            length = 0;
        }

        deque(const deque &other) {
            node *p = head = new node(nodeLength, NULL, NULL);
            node *q = other.head;
            length = other.length;
            p->nodeSize = q->nodeSize;
            if (length == 0) {
                tail = head;
                return;
            }
            while (q != NULL) {
                p->nodeSize = q->nodeSize;
                for (int i = 0; i < q->nodeSize; i++) {
                    p->data[i] = new T(*(q->data[i]));
                }
                p = p->next = new node(nodeLength, p, NULL);
                q = q->next;
            }
            tail = p->prev;
            tail->next = NULL;
            delete p;
        }

        //destruction
        ~deque() {
            if (this != NULL) {
                clearAll();
                delete head;
            }
        }

        //overload operator =
        deque &operator=(const deque &other) {
            if (this == &other) return *this;
            clearAll();
            node *p = head;
            node *q = other.head;
            length = other.length;
            p->nodeSize = q->nodeSize;
            if (length == 0) {
                tail = head;
                return *this;
            }
            while (q != NULL) {
                p->nodeSize = q->nodeSize;
                for (int i = 0; i < p->nodeSize; i++) {
                    p->data[i] = new T(*(q->data[i]));
                }
                p = p->next = new node(nodeLength, p, NULL);
                q = q->next;
            }
            tail = p->prev;
            tail->next = NULL;
            delete p;
            return *this;
        }

        //access specified element with bounds checking
        //throw index_out_of_bound if out of bound.
        T &at(const size_t &pos) {
            if (pos >= length) throw index_out_of_bound();
            node *currentNode;
            int nodePos;
            search(pos, currentNode, nodePos);
            return *(currentNode->data[nodePos]);
        }

        const T &at(const size_t &pos) const {

            if (pos >= length) throw index_out_of_bound();
            node *currentNode;
            int nodePos;

            node *tmp = head;
            unsigned long long counter = pos + 1; //set a counter
            //to find the element in which node
            while (tmp->nodeSize < counter) {
                counter -= tmp->nodeSize;
                tmp = tmp->next;
            }
            currentNode = tmp;
            nodePos = counter - 1;
            //search(int(pos), currentNode, nodePos);
            return *(currentNode->data[nodePos]);
        }

        T &operator[](const size_t &pos) {
            if (pos >= length) throw index_out_of_bound();
            node *currentNode;
            int nodePos;
            search(pos, currentNode, nodePos);
            return *(currentNode->data[nodePos]);
        }

        const T &operator[](const size_t &pos) const {
            if (pos >= length) throw index_out_of_bound();
            node *currentNode;
            int nodePos;

            node *tmp = head;
            unsigned long long counter = pos + 1; //set a counter
            //to find the element in which node
            while (tmp->nodeSize < counter) {
                counter -= tmp->nodeSize;
                tmp = tmp->next;
            }
            currentNode = tmp;
            nodePos = counter - 1;
            //search(pos, currentNode, nodePos);
            return *(currentNode->data[nodePos]);
        }

        //access the first element
        // throw container_is_empty when the container is empty
        const T &front() const {
            if (length == 0) throw container_is_empty();
            return *(head->data[0]);
        }

        //access the last element
        //throw container_is_empty when the container is empty.
        const T &back() const {
            if (length == 0) throw container_is_empty();
            return *(tail->data[tail->nodeSize - 1]);
        }

        //returns an iterator to the beginning.
        iterator begin() {
            iterator it(this, head, 0);
            return it;
        }

        const_iterator cbegin() const {
            const_iterator it(this, head, 0);
            return it;
        }

        //returns an iterator to the end.
        iterator end() {
            iterator it(this, tail, tail->nodeSize);
            return it;
        }

        const_iterator cend() const {
            const_iterator it(this, tail, tail->nodeSize);
            return it;
        }

        //checks whether the container is empty.
        bool empty() const { return length == 0; }

        //returns the number of elements
        size_t size() const { return length; }

        //clears the contents
        void clear() { clearAll(); }

        //inserts elements at the specified location on in the container.
        //inserts value before pos
        //returns an iterator pointing to the inserted value
        //throw if the iterator is invalid or it point to a wrong place.
        iterator insert(iterator pos, const T &value) {
            if (pos.que != this) throw invalid_iterator();
            if (pos.currentNode == NULL) throw invalid_iterator();
            if (pos.currentNode == tail && (pos.nodePos > pos.currentNode->nodeSize
                                            || pos.nodePos < 0))
                throw invalid_iterator();
            if (pos.currentNode != tail && (pos.nodePos >= pos.currentNode->nodeSize
                                            || pos.nodePos < 0))
                throw invalid_iterator();

            length++;

            for (int i = pos.currentNode->nodeSize; i > pos.nodePos; i--) {
                pos.currentNode->data[i] = pos.currentNode->data[i - 1];
                pos.currentNode->data[i - 1] = NULL;
            }
            pos.currentNode->data[pos.nodePos] = new T(value);
            pos.currentNode->nodeSize++;

            if (pos.currentNode->nodeSize == nodeLength) {
                T *place = pos.currentNode->data[pos.nodePos];
                iterator tmp(this, pos.currentNode, 0);
                split(pos.currentNode);

                while (tmp.currentNode->data[tmp.nodePos] != place) {
                    tmp++;
                }
                return tmp;
            } else {
                return pos;
            }

            /*if(tail->nodeSize == nodeLength && pos == this->end()){
                tail = tail->next = new node(nodeLength, tail, NULL);
                tail->data[0] = new T(value);
                tail->nodeSize++;
                pos.currentNode = tail;
                pos.nodePos = 0;
                return pos;
            }
            else if(tail->nodeSize != nodeLength && pos == this->end()){
                tail->data[tail->nodeSize] = new T(value);
                tail->nodeSize++;
                return pos;
            }
            else;

            node *tmpNode = pos.currentNode;
            int tmpNodePos = pos.nodePos;
            if(tmpNode->nodeSize == nodeLength){
                tmpNode->next = new node(nodeLength,tmpNode,tmpNode->next);
                if(tmpNode->next->next) tmpNode->next->next->prev =tmpNode->next;
                for(int i = tmpNodePos;i < tmpNode->nodeSize;i++){
                    tmpNode->next->data[i-tmpNodePos] = new T(*tmpNode->data[i]);
                    delete tmpNode->data[i];
                    tmpNode->data[i] = NULL;
                }
                tmpNode->next->nodeSize = tmpNode->nodeSize - tmpNodePos;
                tmpNode->nodeSize = tmpNodePos + 1;
                //if(tmpNode->data[tmpNodePos]) delete tmpNode->data[tmpNodePos];
                tmpNode->data[tmpNodePos] = new T(value);
                return pos;
            }

            for(int i = tmpNode->nodeSize-1;i >= tmpNodePos;i--){
                tmpNode->data[i+1] = new T(*(tmpNode->data[i]));
                delete tmpNode->data[i];
            }
            tmpNode->nodeSize++;
            tmpNode->data[tmpNodePos] = new T(value);
            return pos;*/
        }

        //removes specified element at pos.
        //removes the element at pos.
        //returns an iterator pointing to the following element, if pos pointing to the last element, end() will be returned.
        // throw if the container is empty, the iterator is invalid or it points to a wrong place.
        iterator erase(iterator pos) {
            if (pos.que != this) throw invalid_iterator();
            if (pos.currentNode == NULL) throw invalid_iterator();
            if (pos.nodePos >= pos.currentNode->nodeSize
                || pos.nodePos < 0)
                throw invalid_iterator();
            if (empty()) throw container_is_empty();

            length--;
            //if the pos points to the last data, i should know that...
            T *place;
            bool isEnd = false;
            if(pos + 1 == end())
                isEnd = true;
            else{
                //if the pos doesn't point to the last data, so i can use place to find...
                place = (pos + 1).currentNode->data[(pos + 1).nodePos];
            }


            //if the node is not deleted, this tmp iterator can be used.
            iterator tmp(pos + 1);
            delete pos.currentNode->data[pos.nodePos];
            pos.currentNode->data[pos.nodePos] = NULL;
            pos.currentNode->nodeSize--;
            //the nodeSize has been  minus...
            for (int i = pos.nodePos; i < pos.currentNode->nodeSize; i++) {
                pos.currentNode->data[i] = pos.currentNode->data[i + 1];
                pos.currentNode->data[i + 1] = NULL;
            }

            //if the deque is empty
            if (length == 0)
                return begin();
            //if not
            else {
                //if this node is empty
                if (pos.currentNode->nodeSize == 0) {
                    //if this node is also the head, delete this node and put the pointer *head to the next node
                    if (pos.currentNode == head) {
                        node *del = head;
                        head = head->next;
                        head->prev = NULL;
                        delete del;

                        return begin();

                    }
                    else{
                        //if this node is tail, delete this node and put the pointer *tail to the previous node
                        if(pos.currentNode == tail){
                            node *del = tail;
                            tail = tail->prev;
                            tail->next = NULL;
                            delete del;

                            return end();
                        }
                        //others, delete this node and change its prev-next & next-prev
                        else{
                            node *del = pos.currentNode;
                            del->prev->next = del->next;
                            del->next->prev = del->prev;
                            delete del;

                            return tmp;

                        }
                    }
                }

                //this node is not empty, so consider whether its next and itself can be merged
                else {
                    //if this node is tail ,it doesn't have next...but it has prev
                    if (pos.currentNode == tail){
                        //if the deque only has one node, nothing should be done.
                        if(pos.currentNode == head)
                            return pos;
                        //else, let's do something
                        else {
                            while (pos.currentNode != head && pos.currentNode->nodeSize < nodeLength / 2 &&
                            pos.currentNode->nodeSize + pos.currentNode->prev->nodeSize < nodeLength) {
                                    for (int i = 0; i < pos.currentNode->nodeSize; i++) {
                                        pos.currentNode->prev->data[pos.currentNode->prev->nodeSize + i] = pos.currentNode->data[i];
                                        pos.currentNode->data[i] = NULL;
                                    }
                                    node *del = pos.currentNode;
                                    pos.currentNode = pos.currentNode->prev;
                                    pos.currentNode->nodeSize += del->nodeSize;
                                    pos.currentNode->next = NULL;
                                    //pos.currentNode->next = del->next;
                                    //del->next->prev = pos.currentNode;
                                    delete del;
                            }
                        }
                        if(isEnd) {
                            tail = pos.currentNode;
                            return end();
                        }
                        else {
                            tail = pos.currentNode;
                            iterator search(this, pos.currentNode, 0);
                            while (search.currentNode->data[search.nodePos] != place)
                                search++;

                            return search;
                        }
                    }
                    //maybe it has chance to be merged ?  backforward and preforward
                    else {
                        //the condition that its next is tail is so mess, put it off
                        //operate these nodes until can't be merged or occur the condition
                        //this condition we should only backforward merge.
                        if(pos.currentNode == head) {
                            while (pos.currentNode->next != tail && pos.currentNode->nodeSize < nodeLength / 2) {
                                if(pos.currentNode == tail)
                                    break;
                                if (pos.currentNode->nodeSize + pos.currentNode->next->nodeSize < nodeLength) {
                                    for (int i = 0; i < pos.currentNode->next->nodeSize; i++) {
                                        pos.currentNode->data[pos.currentNode->nodeSize +
                                                              i] = pos.currentNode->next->data[i];
                                        pos.currentNode->next->data[i] = NULL;
                                    }
                                    node *del = pos.currentNode->next;
                                    pos.currentNode->nodeSize += del->nodeSize;
                                    pos.currentNode->next = del->next;
                                    del->next->prev = pos.currentNode;
                                    delete del;
                                    //pos.currentNode = pos.currentNode->next;
                                }
                                else {
                                    break;
                                }
                            }

                            if (pos.currentNode->next == tail &&
                                pos.currentNode->nodeSize < nodeLength / 2 &&
                                pos.currentNode->nodeSize + pos.currentNode->next->nodeSize < nodeLength) {
                                for (int i = 0; i < tail->nodeSize; i++) {
                                    pos.currentNode->data[pos.currentNode->nodeSize +
                                                          i] = tail->data[i];
                                    tail->data[i] = NULL;
                                }
                                pos.currentNode->nodeSize += tail->nodeSize;
                                node *del = tail;
                                //pos.currentNode->next = NULL;
                                delete del;
                                tail = pos.currentNode;
                                tail->next = NULL;
                                if (isEnd)
                                    return end();
                                else {
                                    node *cmp = pos.currentNode == head ? pos.currentNode : pos.currentNode->prev;
                                    iterator search(this, cmp, 0);
                                    while (search.currentNode->data[search.nodePos] != place)
                                        search++;
                                    return search;
                                }
                            }
                            else {
                                if (isEnd)
                                    return end();
                                else {
                                    node *cmp = pos.currentNode == head ? pos.currentNode : pos.currentNode->prev;
                                    iterator search(this, cmp, 0);
                                    while (search.currentNode->data[search.nodePos] != place)
                                        search++;
                                    return search;
                                }
                                //return pos;
                            }
                        }
                        //not from head, so when next-operation complete, prev-operation start.
                        else{
                            while (pos.currentNode->next != tail && pos.currentNode->nodeSize < nodeLength / 2) {
                                if(pos.currentNode == tail)
                                    break;
                                if (pos.currentNode->nodeSize + pos.currentNode->next->nodeSize < nodeLength) {
                                    for (int i = 0; i < pos.currentNode->next->nodeSize; i++) {
                                        pos.currentNode->data[pos.currentNode->nodeSize +
                                                              i] = pos.currentNode->next->data[i];
                                        pos.currentNode->next->data[i] = NULL;
                                    }
                                    node *del = pos.currentNode->next;
                                    pos.currentNode->nodeSize += del->nodeSize;
                                    pos.currentNode->next = del->next;
                                    del->next->prev = pos.currentNode;
                                    delete del;
                                    //pos.currentNode = pos.currentNode->next;
                                }
                                else {
                                    break;
                                }
                            }

                            bool isTail = false;

                            if (pos.currentNode->next == tail &&
                                pos.currentNode->nodeSize < nodeLength / 2 &&
                                pos.currentNode->nodeSize + pos.currentNode->next->nodeSize < nodeLength) {
                                for (int i = 0; i < pos.currentNode->next->nodeSize; i++) {
                                    pos.currentNode->data[pos.currentNode->nodeSize +
                                                          i] = pos.currentNode->next->data[i];
                                    pos.currentNode->next->data[i] = NULL;
                                }
                                pos.currentNode->nodeSize += tail->nodeSize;
                                node *del = tail;
                                //pos.currentNode->next = NULL;
                                delete del;
                                tail = pos.currentNode;
                                tail->next = NULL;
                                isTail = true;
                            }
                            else {
                                //return pos;
                            }

                            if(isTail){
                                while (pos.currentNode != head && pos.currentNode->nodeSize < nodeLength / 2) {
                                    if (pos.currentNode->nodeSize + pos.currentNode->prev->nodeSize < nodeLength) {
                                        for (int i = 0; i < pos.currentNode->nodeSize; i++) {
                                            pos.currentNode->prev->data[pos.currentNode->prev->nodeSize + i] = pos.currentNode->data[i];
                                            pos.currentNode->data[i] = NULL;
                                        }
                                        node *del = pos.currentNode;
                                        pos.currentNode = pos.currentNode->prev;
                                        pos.currentNode->nodeSize += del->nodeSize;
                                        pos.currentNode->next = NULL;
                                        //pos.currentNode->next = del->next;
                                        //del->next->prev = pos.currentNode;
                                        delete del;
                                    }
                                    else {
                                        break;
                                    }
                                }

                                tail = pos.currentNode;
                                if (isEnd)
                                    return end();
                                else {
                                    node *cmp = pos.currentNode == head ? pos.currentNode : pos.currentNode->prev;
                                    iterator search(this, cmp, 0);
                                    while (search.currentNode->data[search.nodePos] != place)
                                        search++;
                                    return search;
                                }
                            }
                            else{
                                while (pos.currentNode != head && pos.currentNode->nodeSize < nodeLength / 2) {
                                    if (pos.currentNode->nodeSize + pos.currentNode->prev->nodeSize < nodeLength) {
                                        for (int i = 0; i < pos.currentNode->nodeSize; i++) {
                                            pos.currentNode->prev->data[pos.currentNode->prev->nodeSize + i] = pos.currentNode->data[i];
                                            pos.currentNode->data[i] = NULL;
                                        }
                                        node *del = pos.currentNode;
                                        pos.currentNode = pos.currentNode->prev;
                                        pos.currentNode->nodeSize += del->nodeSize;
                                        //pos.currentNode->next = NULL;
                                        pos.currentNode->next = del->next;
                                        del->next->prev = pos.currentNode;
                                        delete del;
                                    }
                                    else {
                                        break;
                                    }
                                }

                                if (isEnd)
                                    return end();
                                else {
                                    node *cmp;
                                    if (pos.currentNode == head) {
                                        cmp = pos.currentNode;
                                    } else {
                                        cmp = pos.currentNode->prev;
                                    }
                                    iterator search(this, cmp, 0);
                                    while (search.currentNode->data[search.nodePos] != place)
                                        search++;
                                    return search;
                                }

                            }
                        }
                    }

                }

            }
        }

        // adds an element to the end
        void push_back(const T &value) {
            insert(end(), value);
        }

        //removes the last element
        //throw when the container is empty.
        void pop_back() {
            erase(end()-1);
        }

        //inserts an element to the beginning.
        void push_front(const T &value) {
            insert(begin(), value);
        }

        //removes the first element.
        //throw when the container is empty.
        void pop_front() {
            erase(begin());
        }



        void split(node *pos) {
            if (pos == tail) {
                tail = tail->next = new node(nodeLength, tail, NULL);
                for (int i = 0; i < nodeLength / 2; i++) {
                    tail->data[i] = pos->data[nodeLength / 2 + i];
                    pos->data[nodeLength / 2 + i] = NULL;
                }
                pos->nodeSize = nodeLength / 2;
                tail->nodeSize = nodeLength / 2;
            } else if (pos == head) {
                pos->next = new node(nodeLength, pos, pos->next);
                node *tmp = pos->next;
                for (int i = 0; i < nodeLength / 2; i++) {
                    tmp->data[i] = pos->data[i + nodeLength / 2];
                    pos->data[i + nodeLength / 2] = NULL;
                }
                tmp->nodeSize = nodeLength / 2;
                pos->nodeSize = nodeLength / 2;
                tmp->next->prev = tmp;
            } else {
                pos->next = new node(nodeLength, pos, pos->next);
                node *tmp = pos->next;
                for (int i = 0; i < nodeLength / 2; i++) {
                    tmp->data[i] = pos->data[i + nodeLength / 2];
                    pos->data[i + nodeLength / 2] = NULL;
                }
                tmp->next->prev = tmp;
                tmp->nodeSize = nodeLength / 2;
                pos->nodeSize = nodeLength / 2;
            }
        }


    };
}

#endif
