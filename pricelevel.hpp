#include <queue>
#include "order.hpp"

class PriceLevel {
  private:
    t_qty totalQty;
    std::queue<Order*> queue;

  public:
    void add(Order*);
    void fill(Order*);
};