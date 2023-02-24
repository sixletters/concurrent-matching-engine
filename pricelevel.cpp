#include "pricelevel.hpp"

void PriceLevel::fill(Order* incomingOrder) {
  Order* restingOrder;
  while (!queue.empty() || !incomingOrder->isDone()) {
    restingOrder = queue.front();
    totalQty -= restingOrder->match(incomingOrder);
    if (restingOrder->isDone())
      queue.pop();
  }
}

void PriceLevel::add(Order* pOrder) {
  totalQty += pOrder->qty; 
  queue.push(pOrder);
}