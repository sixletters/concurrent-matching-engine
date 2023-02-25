#include <algorithm>
#include "pricelevel.hpp"

PriceLevel::PriceLevel() {
  totalQty = 0;
  // initialise threadsafe queue here
};

void PriceLevel::fill(Order* newOrder) {
  t_qty fillQty = std::min(newOrder->qty, totalQty);
  totalQty -= fillQty;
  // new thread to acquire head lock
  // main thread return
  // new thread to continue execution
  Order* restingOrder;
  while (!queue.empty() || !newOrder->isDone()) {
    restingOrder = queue.front();
    totalQty -= newOrder->match(restingOrder);
    if (restingOrder->isDone())
      queue.pop();
  }
}

void PriceLevel::add(Order* pOrder) {
  totalQty += pOrder->qty; 
  // new thread to acquire tail lock
  // main thread return
  // new thread to continue execution
  queue.push(pOrder);
}