#include "queue.hpp"
#include "order.hpp"
#include <string>

class PriceLevel {
  public:
    Queue<Order*> queue;
    t_qty totalQty;
    PriceLevel();

    std::string str() const;

    void add(Order*, const uint32_t);
    void fill(Order*, t_qty, const uint32_t);
    void cancel(Order*, const uint32_t);
};
