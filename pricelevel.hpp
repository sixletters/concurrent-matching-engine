#include <queue>
#include <semaphore>
#include "order.hpp"

class PriceLevel {
  private:
    Queue<Order*> queue;

  public:
    t_qty totalQty;

    PriceLevel();

    void add(Order*);
    void fill(Order*);

    void addAsync(Order*, std::binary_semaphore&);
    void fillAsync(Order*, t_qty, std::binary_semaphore&);
};