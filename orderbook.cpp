#include "orderbook.hpp"
#include <stdexcept>

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
  std::lock_guard<FIFOMutex> lg(orderbookMutex);
  std::vector<std::thread> threads;
  // match order
  {
    PRICELEVELMAP& levelsMap = _oppSide(newOrder->side);
    auto it = levelsMap.begin();
    while (it != levelsMap.end() && newOrder->canMatchPrice(it->first)) {
      PriceLevel* level = it->second;
      const t_qty fillQty = std::min(newOrder->qty, level->totalQty);
      level->totalQty -= fillQty; newOrder->qty -= fillQty;
      level->queue.getFrontMutex().lock();
      auto th = std::thread(&PriceLevel::fill, level, newOrder, fillQty, timestamp);
      threads.push_back(th);
      th.detach();
      if (newOrder->qty == 0) { goto end; }
      it++;
    }
  } 

  // insert order if qty > 0
  {
    PRICELEVELMAP& levelsMap = _sameSide(newOrder->side);
    auto it = levelsMap.find(newOrder->price);
    if (it == levelsMap.end()) { // if price level does not exist
      auto level = new PriceLevel();
      auto ret = levelsMap.insert(std::pair{newOrder->price, level});
      it = ret.first;
    }

    PriceLevel* level = it->second;
    level->totalQty += newOrder->qty;
    level->queue.getBackMutex().lock();

    auto th = std::thread(&PriceLevel::add, level, newOrder, timestamp);
    threads.push_back(th);
    th.detach();
  }

  end:
    for (std::thread& th : threads) th.join();
  // print();
}

// sequential
void Orderbook::cancelOrder(Order* order, uint32_t timestamp) {
  std::lock_guard<FIFOMutex> lg(orderbookMutex);
  PriceLevel* level = _sameSide(order->side)[order->price];
  std::lock_guard<std::mutex> lg(level->queue.getFrontMutex()); // make sure no other thread is filling
  if (order->qty == 0) { // if done
    Output::OrderDeleted(order->ID, false, timestamp);
    return;
  }
  level->totalQty -= order->qty;
  order->qty = 0;
  Output::OrderDeleted(order->ID, true, timestamp);
  // print();
}
