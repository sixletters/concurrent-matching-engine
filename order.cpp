#include <algorithm>
#include <order.hpp>
#include <stdexcept>

Order::Order(const t_client _client, const t_orderid _id, const SIDE _side, 
              const t_qty _qty, const t_price _price) : 
              ID(_id), client(_client), side(_side), price(_price) {
    qty = _qty;
    executionID = 0;
}

void Order::cancel() {
  if (qty == 0) {
    Output::OrderDeleted(ID, false, 420);
    return;
  }
  qty = 0;
  Output::OrderDeleted(ID, false, 420);
}

t_qty Order::match(Order* const restingOrder) {
    if (isDone() || restingOrder->isDone()) return 0;

    t_qty fillQty = std::min(qty, restingOrder->qty);
    restingOrder->qty -= fillQty;
    qty -= fillQty;
    restingOrder->executionID++;
    Output::OrderExecuted(restingOrder->ID, ID, restingOrder->executionID, restingOrder->price, fillQty, 420);
    return fillQty;
}

bool Order::isDone() const { return qty == 0; }

bool Order::canMatchPrice(const t_price restingPrice) const { 
  switch (side) {
    case SIDE::BUY: return price >= restingPrice;
    case SIDE::SELL: return price <= restingPrice;
    default: throw std::runtime_error("");
  }
}
