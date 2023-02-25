#include <semaphore>
#include "queue.hpp"
#include "order.hpp"
#include <thread>

class PriceLevel {
  private:
    Queue<Order*> queue;
    std::binary_semaphore sem;

  public:
    t_qty totalQty;

    PriceLevel();

    void add(Order*);
    void fill(Order*);

    void addAsync(Order*);
    void fillAsync(Order*, t_qty);
};