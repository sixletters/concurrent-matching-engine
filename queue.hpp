#include <memory>
#include <mutex>
#include "order.hpp"
template<typename T>
class Queue{
    private:
        struct Node{
            T* order_data;
            std::unique_ptr<Node> next;
            Node(T* data_):
                order_data(data_)
            {}
        };
        Node* front;
        Node* back;
        std::mutex head_mutex;
        std::mutex tail_mutex
    public:
    Queue(): front(new Node), back(front.get()){};
    // delete the copy assignment operator
    Queue(const queue&other)=delete;
    Queue& operator=(const queue& other) = delete;
    std::mutex* getHeadMutex();
    std::mutex* getTailMutex();
    void pop();
    void push(T*);
    T& front();
    int size();
    bool empty();
};