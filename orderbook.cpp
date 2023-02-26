#include "orderbook.hpp"
#include <stdexcept>

Orderbook::Orderbook(const std::string _instrument) : _bids(false), _asks(true), instrument(_instrument) {}

/* Print orderbook state */
void Orderbook::print() const {
  std::string output = "";
  {
    for (auto it = _asks.rbegin(); it != _asks.rend(); it++) {
      output += "$" + std::to_string(it->first) + " x " + it->second->str();
    }
  }
    SyncCerr {} << "----------------------\n";
  {
    for (auto it = _bids.begin(); it != _bids.end(); it++) {
      output += "$" + std::to_string(it->first) + " x " + it->second->str();
    }
  }
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

void Orderbook::createOrder(Order* const newOrder, uint32_t timestamp) {
  std::lock_guard<FIFOMutex> lg(orderbookLock);
  // match order
  {
    PL_MAP& levels = _oppSide(newOrder->side);
    auto it = levels.begin();
    while (it != levels.end() && newOrder->canMatchPrice(it->first)) {
      PriceLevel* pl = it->second;
      pl->fill(newOrder, timestamp++);
      if (newOrder->qty == 0) return;
      it++;
    }
  } 

  // insert order if qty > 0
  {
    PL_MAP& levels = _sameSide(newOrder->side);
    auto it = levels.find(newOrder->price);
    if (it != levels.end()) { // if price level exists
      it->second->add(newOrder, timestamp); 
    } else { // create new level
      auto level = new PriceLevel();
      level->add(newOrder, timestamp);
      levels.insert(std::pair{newOrder->price, level});
    } 
  }
}

void Orderbook::cancelOrder(Order* order, uint32_t timestamp) {
  std::lock_guard<FIFOMutex> lg(orderbookLock);
  PriceLevel* pl = _sameSide(order->side)[order->price];
  pl->cancel(order, timestamp);
}
