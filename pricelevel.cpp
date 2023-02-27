#include <algorithm>
#include <thread>
#include <sstream>
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
    OrderExecuted(restingOrder, newOrder, fillQty, t);
    if (restingOrder->qty == 0) queue.pop();
  }
  queue.unlockFront();
}

void PriceLevel::add(Order* newOrder, const uint32_t t) { 
  queue.push(newOrder); 
  OrderAdded(newOrder, t);
  queue.unlockBack();
}

std::string OrderAdded(Order* order, uint32_t timestamp) {
  std::stringstream ss;
  ss << (order->side == SIDE::SELL ? "S " : "B " ) //
    << (order->ID) << " " //
    << (order->instrument) << " " //
    << (order->price) << " " //
    << (order->qty) << " " //
    << (timestamp); //
  return ss.str();
}

std::string OrderExecuted(Order* resting, Order* incoming, t_qty qty, uint32_t timestamp){
  std::stringstream ss;
  ss << "E " //
    << (resting->ID) << " " //
    << (incoming->ID) << " " //
    << (resting->executionID) << " " //
    << (resting->price) << " " //
    << (qty) << " " //
    << (timestamp); //
  return ss.str();
}