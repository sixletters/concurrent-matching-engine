#include <stdint.h>
#include <string>
#include <vector>
#include "types.hpp"
#include "order.hpp"
#include "orderbook.hpp"

Orderbook::Orderbook(const std::string symbol) : symbol(symbol) {}

/* Print orderbook state */
void Orderbook::print() const {
  std::printf("----------------\n");
  std::printf("----------------\n");
  std::printf("----------------\n\n");
}

std::map<t_price, std::queue<Order*>*>& Orderbook::_oppSide(const Side side) {
  switch (side) {
    case Side::BUY: return _asks;
    case Side::SELL: return _bids;
    // default: throw std::runtime_error("invalid side");
  }
}

std::map<t_price, std::queue<Order*>*>& Orderbook::_sameSide(const Side side) {
  switch (side) {
    case Side::BUY: return _bids;
    case Side::SELL: return _asks;
    // default: throw std::runtime_error("invalid side");
  }
}

void Orderbook::createOrder(const t_client client, const Side side, const t_qty qty, const t_price price) {
  Order* pOrder = new Order(client, side, qty, price);
  _allOrders.push_back(pOrder);

  matchOrder(_oppSide(side), *pOrder);

  if (pOrder->isDone()) return;

  std::map<t_price, std::queue<Order*>*>& levels = _sameSide(side);
  auto it = levels.find(price);
  if (it != levels.end()) {
    it->second->push(pOrder);
  } else {
    // create new level
    auto q = new std::queue<Order*>();
    q->push(pOrder);
    levels.insert(std::pair{price, q});
  }
}

/* lazy cancellation */
void Orderbook::cancelOrder(const t_client client, const t_orderid id) {
  Order& order = *_allOrders.at(id);
  if (order.client != client) {
    // reject(*this, "Unauthorised");
    return;
  } 
  order.cancel();
}

/* matches order, O(n + m)
  where n = number of orders, m = number of price levels */
void Orderbook::matchOrder(std::map<t_price, std::queue<Order*>*>& levels, Order& incomingOrder) {
}
