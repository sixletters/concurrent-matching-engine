#include <algorithm>
#include <thread>
#include "pricelevel.hpp"

PriceLevel::PriceLevel() : totalQty(0) {};

void PriceLevel::fill(Order* const newOrder, t_qty levelFillQty, const uint32_t t) { 
  Order* restingOrder;
  while (levelFillQty > 0) {
    restingOrder = queue.front();
    t_qty fillQty = std::min(levelFillQty, restingOrder->qty);
    levelFillQty -= fillQty;
    restingOrder->qty -= fillQty;
    restingOrder->executionID++;
    Output::OrderExecuted(restingOrder->ID, newOrder->ID, restingOrder->executionID, restingOrder->price, fillQty, t);
    if (restingOrder->qty == 0) queue.pop();
  }
  queue.unlockFront();
}

void PriceLevel::add(Order* newOrder, const uint32_t t) { 
  queue.push(newOrder); 
  Output::OrderAdded(newOrder->ID, newOrder->instrument.c_str(), newOrder->price, newOrder->qty, newOrder->side == SIDE::SELL, t);
  queue.unlockBack();
}

std::string PriceLevel::str() const {
  return std::to_string(totalQty);
}
