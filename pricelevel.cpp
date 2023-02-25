#include <algorithm>
#include "pricelevel.hpp"

PriceLevel::PriceLevel() : queue(Queue<Order*>{}), 
  sem(std::binary_semaphore{0}), totalQty(0){};

void PriceLevel::fill(Order* newOrder) {
  t_qty fillQty = std::min(newOrder->qty, totalQty);
  totalQty -= fillQty;
  newOrder->qty -= fillQty;
	auto thread = std::thread(&PriceLevel::fillAsync, this, newOrder, fillQty);
	thread.detach();
  sem.acquire();
}

void PriceLevel::fillAsync(Order* newOrder, t_qty levelFillQty) {
  std::lock_guard<std::mutex> lg(*(queue.getFrontMutex()));
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
	auto thread = std::thread(&PriceLevel::fillAsync, this, newOrder);
	thread.detach();
  sem.acquire();
}

void PriceLevel::addAsync(Order* newOrder) {
  std::lock_guard<std::mutex> lg(*(queue.getBackMutex()));
  sem.release(); // signal back to main thread

  queue.push(newOrder);
}