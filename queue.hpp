#include <memory>
#include <mutex>
#include "order.hpp"
#include <atomic>
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
        std::atomic<int> size;
        std::unique_ptr<Node> front_ptr;
        Node* back;
        std::mutex front_mutex;
        std::mutex back_mutex;
    public:
    Queue(): size(std::atomic<int>{0}),front_ptr(new Node(nullptr)), back(front_ptr.get()){};
    // delete the copy constructor.
    Queue(const Queue&other)=delete;
    // delete the copy assignment operator.
    Queue& operator=(const Queue& other) = delete;
    void pop(){
        if(front_ptr.get() == back){
            return;
        }
        std::unique_ptr<Node> old_front=std::move(front_ptr);
        front_ptr=std::move(old_front->next);
        size.fetch_sub(1,std::memory_order_release);
    }
    void push(T order_data){
        std::unique_ptr<Node> p(new Node);
        Node* const new_back=p.get();
        back->order_data = order_data;
        back->next=std::move(p);
        back=new_back;
        size.fetch_add(1,std::memory_order_release);
    };
    bool empty(){
        if(front_ptr.get() == back){
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

    int getSize(){
        return size.load();
    }
};
