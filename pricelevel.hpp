#include "queue.hpp"
#include "order.hpp"
#include <thread>

class PriceLevel {
  public:
    Queue<Order*> queue;
    t_qty totalQty;

    PriceLevel();

    void add(Order*);
    void fill(Order*, const uint32_t);

    void addAsync(Order*);
    void fillAsync(Order*, t_qty, const uint32_t);
};
