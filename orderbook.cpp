#include "orderbook.hpp"
#include <stdexcept>
#include <vector>

Orderbook::Orderbook(const std::string _instrument) : _bids(false), _asks(true), instrument(_instrument) {}

/* Print orderbook state */
void Orderbook::print() const {
  std::string output = "[ ";
  {
    for (auto it = _asks.rbegin(); it != _asks.rend(); it++) {
      output += "$" + std::to_string(it->first) + "x" + it->second->str() + ", ";
    }
  }
    output += " | ";
  {
    for (auto it = _bids.begin(); it != _bids.end(); it++) {
      output += "$" + std::to_string(it->first) + "x" + it->second->str() + ", ";
    }
  }
  output += "]";
  SyncCerr {} << output << std::endl;
}

PRICELEVELMAP& Orderbook::_oppSide(const SIDE side) {
  switch (side) {
    case SIDE::BUY: return _asks;
    case SIDE::SELL: return _bids;
    default: throw std::runtime_error("");
  }
}

PRICELEVELMAP& Orderbook::_sameSide(const SIDE side) {
  switch (side) {
    case SIDE::BUY: return _bids;
    case SIDE::SELL: return _asks;
    default: throw std::runtime_error("");
  }
}

// parallel
void Orderbook::createOrder(Order* const newOrder, uint32_t timestamp) {
  std::lock_guard<FIFOMutex> orderbookLock(orderbookMutex);
  // match order
  {
    PRICELEVELMAP& levelsMap = _oppSide(newOrder->side);
    for (auto it = levelsMap.begin(); it != levelsMap.end() && newOrder->canMatchPrice(it->first); it++) {
      PriceLevel* level = it->second;

      const t_qty fillQty = std::min(newOrder->qty, level->totalQty);
      level->totalQty -= fillQty; newOrder->qty -= fillQty;

      level->queue.lockFront();
      auto th = std::thread(&PriceLevel::fill, level, newOrder, fillQty, timestamp);
      th.detach();

      if (newOrder->qty == 0) return;
    }
  } 

  // insert order if qty > 0
  {
    PRICELEVELMAP& levelsMap = _sameSide(newOrder->side);
    auto it = levelsMap.find(newOrder->price);
    if (it == levelsMap.end()) { // if price level does not exist
      auto ret = levelsMap.insert(std::pair{newOrder->price, new PriceLevel()});
      it = ret.first;
    }

    PriceLevel* level = it->second;
    level->totalQty += newOrder->qty;

    level->queue.lockBack(); 
    auto th = std::thread(&PriceLevel::add, level, newOrder, timestamp);
    th.detach();
  } 
  // print();
}

// sequential
void Orderbook::cancelOrder(Order* order, uint32_t timestamp) {
  std::lock_guard<FIFOMutex> orderbookLock(orderbookMutex);
  PriceLevel* level = _sameSide(order->side)[order->price];
  std::lock_guard<std::mutex> lg(level->queue.frontMutex); // ensure no other thead is filling
  if (order->qty == 0) { 
    Output::OrderDeleted(order->ID, false, timestamp);
  } else {
    level->totalQty -= order->qty;
    order->qty = 0;
    Output::OrderDeleted(order->ID, true, timestamp);
  } 
  // print();
}
