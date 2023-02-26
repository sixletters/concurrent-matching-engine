#include "queue.hpp"
#include "order.hpp"
#include <string>

class PriceLevel {
  private:
    Queue<Order*> queue;
    t_qty totalQty;

  public:
    PriceLevel();

    std::string str() const;

    void add(Order*, const uint32_t);
    void fill(Order*, const uint32_t);
    void cancel(Order*, const uint32_t);

    void addAsync(Order*, const uint32_t);
    void fillAsync(Order*, t_qty, const uint32_t);
};
