#include <memory>
#include <mutex>
#include "order.hpp"
template<typename T>
class Queue{
    private:
        struct Node{
            T order_data;
            std::unique_ptr<Node> next;
            Node(T data_):
                order_data(data_)
            {};
            Node() {
                order_data = nullptr;
            };
        };
        std::unique_ptr<Node> front_ptr;
        Node* back;
        std::mutex front_mutex;
        std::mutex back_mutex;
    public:
    Queue(): front_ptr(new Node(nullptr)), back(front_ptr.get()){};
    // delete the copy constructor.
    Queue(const Queue&other)=delete;
    // delete the copy assignment operator.
    Queue& operator=(const Queue& other) = delete;
    Node* get_back(){
        std::lock_guard<std::mutex> back_lock(back_mutex);
        return back;
    }
    void pop(){
        std::lock_guard<std::mutex> front_lock(front_mutex);
        if(front_ptr.get() == get_back()){
            return;
        }
        std::unique_ptr<Node> old_front=std::move(front_ptr);
        front_ptr=std::move(old_front->next);
    }
    void push(T order_data){
        std::unique_ptr<Node> p(new Node);
        Node* const new_back=p.get();
        std::lock_guard<std::mutex> back_lock(back_mutex);
        back->order_data = order_data;
        back->next=std::move(p);
        back=new_back;
    };
    bool empty(){
        std::lock_guard<std::mutex> front_lock(front_mutex);
        if(front_ptr.get() == get_back()){
            return true;
        }
        return false;
    }
    T front(){
        if(empty()){
            return nullptr;
        }
        return front_ptr->order_data;
    }

    std::mutex* getFrontMutex(){
        return &front_mutex;
    }

    std::mutex* getBackMutex(){
        return &back_mutex;
    }
};