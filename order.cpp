#include <algorithm>
#include <order.hpp>
#include <stdexcept>

Order::Order(const t_client _client, const t_orderid _id, const SIDE _side, 
              const t_qty _qty, const t_price _price) : 
              ID(_id), client(_client), side(_side), price(_price) {
    qty = _qty;
    executionID = 0;
}

void Order::cancel(const t_client _client) {
  if (qty == 0 || client != _client) {
    Output::OrderDeleted(ID, false, 420);
    return;
  }
  qty = 0;
  Output::OrderDeleted(ID, true, 420);
}

bool Order::isDone() const { return qty == 0; }

bool Order::canMatchPrice(const t_price restingPrice) const { 
  switch (side) {
    case SIDE::BUY: return price >= restingPrice;
    case SIDE::SELL: return price <= restingPrice;
    default: throw std::runtime_error("");
  }
}
