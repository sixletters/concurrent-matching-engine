#include <string>
#include <vector>
#include "queue.hpp"
#include "order.hpp"

class PriceLevel {
  public:
    Queue<Order*> queue;
    t_qty totalQty;
    PriceLevel();

    std::vector<std::string>* add(Order*, const uint32_t);
    std::vector<std::string>* fill(Order*, t_qty, const uint32_t);
};
