#include <algorithm>
#include "pricelevel.hpp"

PriceLevel::PriceLevel() {
  totalQty = 0;
  // initialise threadsafe queue here
};

void PriceLevel::fill(Order* newOrder) {
  t_qty fillQty = std::min(newOrder->qty, totalQty);
  totalQty -= fillQty;
  newOrder->qty -= fillQty;
  std::binary_semaphore sem{0};
	auto thread = std::thread(&PriceLevel::fillAsync, this, newOrder, fillQty, sem);
	thread.detach();
  sem.acquire();
}

void PriceLevel::fillAsync(Order* newOrder, t_qty levelFillQty, std::binary_semaphore& sem) {
  std::lock_guard<std::mutex> lg(queue.getFrontLock());
  sem.release(); // signal back to main thread

  Order* restingOrder;
  while (levelFillQty > 0) {
    restingOrder = queue.front();
    t_qty fillQty = std::min(levelFillQty, restingOrder->qty);
    levelFillQty -= fillQty;
    restingOrder->qty -= fillQty;
    restingOrder->executionID++;
    Output::OrderExecuted(restingOrder->ID, newOrder->ID, restingOrder->executionID, restingOrder->price, fillQty, 420);
    if (restingOrder->isDone()) queue.pop();
  }
}

void PriceLevel::add(Order* newOrder) {
  totalQty += newOrder->qty; 
  std::binary_semaphore sem{0};
	auto thread = std::thread(&PriceLevel::fillAsync, this, newOrder, sem);
	thread.detach();
  sem.acquire();
}

void PriceLevel::addAsync(Order* newOrder, std::binary_semaphore& sem) {
  std::lock_guard<std::mutex> lg(queue.getBackLock());
  sem.release(); // signal back to main thread

  queue.push(newOrder);
}