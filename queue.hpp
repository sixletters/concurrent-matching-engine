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
        std::unique_ptr<Node> front;
        Node* back;
        std::mutex front_mutex;
        std::mutex back_mutex
    public:
    Queue(): front(new Node), back(front.get()){};
    // delete the copy constructor.
    Queue(const queue&other)=delete;
    // delete the copy assignment operator.
    Queue& operator=(const queue& other) = delete;
    std::mutex* getFrontMutex();
    std::mutex* getBackMutex();
    void pop();
    void push(T*);
    T& front();
    bool empty();
};