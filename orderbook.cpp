#include "orderbook.hpp"
#include <stdexcept>

Orderbook::Orderbook(const std::string _instrument) : _bids(false), _asks(true), instrument(_instrument) {}

/* Print orderbook state */
void Orderbook::print() const {
  std::printf("----------------\n");
  {
    auto it = _asks.rbegin();
    while (it != _asks.rend()) {
      std::cout << "$" << it->first << " x " << it->second->totalQty << "\n";
      it++;
    }
  }
  std::printf("----------------\n");
    auto it = _bids.begin();
    while (it != _bids.end()) {
      std::cout << "$" << it->first << " x " << it->second->totalQty << "\n";
      it++;
    }
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

void Orderbook::createOrder(Order* const newOrder, uint32_t timestamp) {
  std::lock_guard<std::mutex> lg(orderbookLock);

  // match order
  {
    PL_MAP& levels = _oppSide(newOrder->side);
    auto it = levels.begin();
    while (it != levels.end() && newOrder->canMatchPrice(it->first)) {
      PriceLevel* pl = it->second;
      pl->fill(newOrder, timestamp++);
      if (newOrder->qty == 0) {
        return;
      };
      it++;
    }
  }


  // insert order if qty > 0
  {
    PL_MAP& levels = _sameSide(newOrder->side);
    auto it = levels.find(newOrder->price);
    if (it != levels.end()) { // if price level exists
      it->second->add(newOrder); 
    } else { // create new level
      auto level = new PriceLevel();
      level->add(newOrder);
      levels.insert(std::pair{newOrder->price, level});
    }
    Output::OrderAdded(newOrder->ID, instrument.c_str(), newOrder->price, newOrder->qty, newOrder->side == SIDE::SELL, timestamp);
  }
}

void Orderbook::cancelOrder(Order* order, uint32_t timestamp) {
  std::lock_guard<std::mutex> lg(orderbookLock);
  if (order->qty == 0) { // if done
    Output::OrderDeleted(order->ID, false, timestamp);
    return;
  }
  PL_MAP& levels = _sameSide(order->side);
  levels[order->price]->totalQty-=order->qty;
  order->qty = 0;
  Output::OrderDeleted(order->ID, true, timestamp);
}