#include <queue>
#include "order.hpp"

class PriceLevel {
  private:
    std::queue<Order*> queue;

  public:
    t_qty totalQty;

    PriceLevel();

    void add(Order*);
    void fill(Order*);
};