#include <mutex>
#include <atomic>
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
    std::mutex frontMutex;
    std::mutex backMutex;

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

    bool empty() const { return pFront.get() == pBack; } 

    T front() const {
      if (empty()) throw std::runtime_error("front() called on empty queue");
      return pFront->data;
    }

    std::mutex& getFrontMutex() {
      return frontMutex;
     }

    std::mutex& getBackMutex() {
      return backMutex;
    } 
};
