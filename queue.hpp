#include <memory>
#include <mutex>
#include "order.hpp"
#include <atomic>
template<typename T>
class Queue{
  private:
    struct Node{
        T data;
        std::unique_ptr<Node> next;
        Node() { data = nullptr; } // dummy
    };
    std::unique_ptr<Node> pFront;
    Node* pBack;
    std::mutex front_mutex;
    std::mutex back_mutex;

  public:
    Queue(): pFront(new Node()), pBack(pFront.get()){};
    ~Queue() = default;

    Queue(const Queue&) = delete;
    Queue& operator=(const Queue&) = delete;
    Queue(Queue&&) = delete;
    Queue& operator=(Queue&&) = delete;

    void pop() {
        if (pFront.get() == pBack) return;
        std::unique_ptr<Node> old_front = std::move(pFront);
        pFront = std::move(old_front->next);
    }

    void push(T data) {
        std::unique_ptr<Node> p(new Node());
        pBack->data = data;
        pBack->next = std::move(p);
        pBack = (pBack->next).get();
    };

    bool empty() { return pFront.get() == pBack; } 

    T front(){
        if (empty()){
            return nullptr;
        }
        return pFront->data;
    }

    std::mutex* getFrontMutex(){
        return &front_mutex;
    }

    std::mutex* getBackMutex(){
        return &back_mutex;
    } 
};
