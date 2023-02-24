#include "orderbook.hpp"

Orderbook::Orderbook(const std::string symbol) : symbol(symbol) {}

/* Print orderbook state */
void Orderbook::print() const {
  std::printf("----------------\n");
  std::printf("----------------\n");
  std::printf("----------------\n\n");
}

std::map<t_price, PriceLevel*>& Orderbook::_oppSide(const Side side) {
  switch (side) {
    case Side::BUY: return _asks;
    case Side::SELL: return _bids;
    // default: throw std::runtime_error("invalid side");
  }
}

std::map<t_price, PriceLevel*>& Orderbook::_sameSide(const Side side) {
  switch (side) {
    case Side::BUY: return _bids;
    case Side::SELL: return _asks;
    // default: throw std::runtime_error("invalid side");
  }
}

void Orderbook::createOrder(const t_client client, const Side side, const t_qty qty, const t_price price) {
  Order* pOrder = new Order(client, side, qty, price);
  _allOrders.push_back(pOrder);

  matchOrder(_oppSide(side), pOrder);

  if (pOrder->isDone()) return;

  std::map<t_price, PriceLevel*>& levels = _sameSide(side);
  auto it = levels.find(price);
  if (it != levels.end()) { // if price level exists
    it->second->add(pOrder); 
  } else { // create new level
    auto level = new PriceLevel();
    level->add(pOrder);
    levels.insert(std::pair{price, level});
  }
}

void Orderbook::cancelOrder(const t_client client, const t_orderid id) {
  Order& order = *_allOrders.at(id);
  if (order.client != client) return; // reject
  order.cancel();
}

void Orderbook::matchOrder(std::map<t_price, PriceLevel*>& levels, Order* incomingOrder) {
  auto it = levels.begin();
  while (it != levels.end() && incomingOrder->price >= it->first) {
    it->second->fill(incomingOrder);
    if (incomingOrder->isDone()) {
      levels.erase(levels.begin(), it); // erase all empty levels
      return;
    };
    it++;
  }
  levels.erase(levels.begin(), it);
}
