#include <algorithm>
#include <order.hpp>

Order::Order(t_client client, t_orderid id, t_qty qty, t_price price) : ID(id), client(client), price(price), qty(qty) {
    _leavesQty = qty;
}

void Order::cancel() {
  if (_leavesQty == 0) return; // reject
  _leavesQty = 0;
}

t_qty Order::match(Order* bookOrder) {
    if (isDone() || bookOrder->isDone()) return;

    t_qty fillQty = std::min(_leavesQty, bookOrder->_leavesQty);
    bookOrder->_leavesQty -= fillQty;
    _leavesQty -= fillQty;
    return fillQty;
}

bool Order::isDone() { return _leavesQty == 0; }
