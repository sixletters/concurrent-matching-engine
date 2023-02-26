#include <algorithm>
#include "pricelevel.hpp"

PriceLevel::PriceLevel() : queue(Queue<Order*>{}), totalQty(0) {};

void PriceLevel::fill(Order* const newOrder, const uint32_t timestamp) {
  t_qty fillQty = std::min(newOrder->qty, totalQty);
  totalQty -= fillQty;
  newOrder->qty -= fillQty;
  queue.getFrontMutex()->lock();
	auto thread = std::thread(&PriceLevel::fillAsync, this, newOrder, fillQty, timestamp);
	thread.detach();
}

void PriceLevel::fillAsync(Order* const newOrder, t_qty levelFillQty, const uint32_t timestamp) { 
  Order* restingOrder;
  while (levelFillQty > 0) {
    restingOrder = queue.front();
    t_qty fillQty = std::min(levelFillQty, restingOrder->qty);
    levelFillQty -= fillQty;
    restingOrder->qty -= fillQty;
    restingOrder->executionID++;
    Output::OrderExecuted(restingOrder->ID, newOrder->ID, restingOrder->executionID, restingOrder->price, fillQty, timestamp);
    if (restingOrder->qty == 0) queue.pop();
  }
  queue.getFrontMutex()->unlock();
}

void PriceLevel::add(Order* newOrder) {
  totalQty += newOrder->qty; 
  queue.getBackMutex()->lock();
	auto thread = std::thread(&PriceLevel::addAsync, this, newOrder);
	thread.detach();
}

void PriceLevel::addAsync(Order* newOrder) { 
  queue.push(newOrder);
  queue.getBackMutex()->unlock();
}
