#include <memory>
#include <mutex>
#include "order.hpp"
template<typename T>
class Queue{
    private:
        struct Node{
            T order_data;
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
     public:
    Node* get_back(){
        std::lock_guard<std::mutex> back_lock(back_mutex);
        return back;
    }
    void pop(){
        std::lock_guard<std::mutex> front_lock(front_mutex);
        if(front.get() == get_back()){
            return
        }
        std::unique_ptr<Node> old_front=std::move(front);
        front=std::move(old_front->next);
    }
    void push(T order_data){
        std::unique_ptr<Node> p(new Node);
        Node* const new_back=p.get();
        std::lock_guard<std::mutex> back_lock(back_mutex);
        back->order_data = order_data
        back->next=std::move(p);
        back=new_back;
    };
    bool empty(){
        std::lock_guard<std::mutex> front_lock(front_mutex);
        if(front.get() == get_back()){
            return true;
        }
        return false;
    }
    T front(){
        if(empty()){
            return nullptr;
        }
        return front->order_data;
    }

    std::mutex* getFrontMutex(){
        return &front_mutex;
    }

    std::mutex* getBackMutex(){
        return &back_mutex
    }
};