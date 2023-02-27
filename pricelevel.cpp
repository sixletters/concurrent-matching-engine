#include <algorithm>
#include <thread>
#include "pricelevel.hpp"

PriceLevel::PriceLevel() : queue(Queue<Order*>{}), totalQty(0) {};

void PriceLevel::fill(Order* const newOrder, t_qty levelFillQty, const uint32_t timestamp) { 
  Order* restingOrder;
  while (levelFillQty > 0) {
    if (queue.empty()) { std::lock_guard<std::mutex>(queue.getBackMutex()); }
    restingOrder = queue.front();
    t_qty fillQty = std::min(levelFillQty, restingOrder->qty);
    levelFillQty -= fillQty;
    restingOrder->qty -= fillQty;
    restingOrder->executionID++;
    Output::OrderExecuted(restingOrder->ID, newOrder->ID, restingOrder->executionID, restingOrder->price, fillQty, timestamp);
    if (restingOrder->qty == 0) queue.pop();
  }
  queue.getFrontMutex().unlock();
}

void PriceLevel::add(Order* newOrder, const uint32_t timestamp) { 
  queue.push(newOrder); 
  Output::OrderAdded(newOrder->ID, newOrder->instrument.c_str(), newOrder->price, newOrder->qty, newOrder->side == SIDE::SELL, timestamp);
  queue.getBackMutex().unlock();
}

std::string PriceLevel::str() const {
  return std::to_string(totalQty);
}
