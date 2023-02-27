#include <mutex>
#include <atomic>
#include <condition_variable>
#include <stdexcept>

template<typename T>
class Queue{
  private:
    struct Node{
      T data;
      std::unique_ptr<Node> next;
    };
    std::unique_ptr<Node> pFront;
    Node* pBack;

    std::condition_variable emptyCv;

  public:
    std::mutex frontMutex;
    std::mutex backMutex;

    Queue(): pFront(new Node()), pBack(pFront.get()) {};
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
      emptyCv.notify_all();
    };

    bool empty() const { return pFront.get() == pBack; } 

    T front() { 
      std::unique_lock<std::mutex> lk{backMutex};
      while (empty()) { emptyCv.wait(lk); };
      return pFront->data;
    }

    void lockFront() { frontMutex.lock(); }
    void unlockFront() { frontMutex.unlock(); }
    void lockBack() { backMutex.lock(); }
    void unlockBack() { backMutex.unlock(); }
};
