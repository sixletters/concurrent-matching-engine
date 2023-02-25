#include "orderbook.hpp"
#include <stdexcept>

Orderbook::Orderbook(const std::string _instrument) : _bids(false), _asks(true), instrument(_instrument) {}

/* Print orderbook state */
void Orderbook::print() const {
  std::printf("----------------\n");
  std::printf("----------------\n");
  std::printf("----------------\n\n");
}

PL_MAP& Orderbook::_oppSide(const SIDE side) {
  switch (side) {
    case SIDE::BUY: return _asks;
    case SIDE::SELL: return _bids;
    default: throw std::runtime_error("");
  }
}

PL_MAP& Orderbook::_sameSide(const SIDE side) {
  switch (side) {
    case SIDE::BUY: return _bids;
    case SIDE::SELL: return _asks;
    default: throw std::runtime_error("");
  }
}

void Orderbook::createOrder(const t_client client, const t_orderid ID, const SIDE side, const t_qty qty, const t_price price) {
  std::lock_guard<std::mutex> lg(global_lock);
  Order* newOrder = new Order(client, ID, side, qty, price);
  _allOrders[ID] = newOrder;

  // match order
  {
    PL_MAP& levels = _oppSide(side);
    auto it = levels.begin();
    while (it != levels.end() && newOrder->canMatchPrice(it->first)) {
      PriceLevel* pl = it->second;
      pl->fill(newOrder);
      if (newOrder->isDone()) {
        levels.erase(levels.begin(), it); // erase all empty levels
        return;
      };
      it++;
    }
    levels.erase(levels.begin(), it); // erase all empty levels
  }


  // insert order if qty > 0
  {
    PL_MAP& levels = _sameSide(side);
    auto it = levels.find(price);
    if (it != levels.end()) { // if price level exists
      it->second->add(newOrder); 
    } else { // create new level
      auto level = new PriceLevel(newOrder);
      levels.insert(std::pair{price, level});
    }
    Output::OrderAdded(ID, instrument.c_str(), price, newOrder->qty, side, 420);
  }
}

void Orderbook::cancelOrder(const t_client client, const t_orderid id) {
  std::lock_guard<std::mutex> lg(global_lock);
  return _allOrders[id]->cancel();
}
