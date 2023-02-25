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

  std::cout<< "HERE";
  Order* restingOrder;
  std::cout<<levelFillQty<< "QTY"<< std::endl;
  while (levelFillQty > 0) {
      std::cout<< "HERE5";
    restingOrder = queue.front();
      std::cout<< "HERE6";
    t_qty fillQty = std::min(levelFillQty, restingOrder->qty);
    levelFillQty -= fillQty;
    restingOrder->qty -= fillQty;
    restingOrder->executionID++;
      std::cout<< "HERE1";
    Output::OrderExecuted(restingOrder->ID, newOrder->ID, restingOrder->executionID, restingOrder->price, fillQty, 420);
      std::cout<< "HERE2";
    if (restingOrder->isDone()) queue.pop();
  }
}

void PriceLevel::add(Order* newOrder) {
  totalQty += newOrder->qty; 
	auto thread = std::thread(&PriceLevel::addAsync, this, newOrder);
	thread.detach();
  sem.acquire();
}

void PriceLevel::addAsync(Order* newOrder) {
  std::lock_guard<std::mutex> lg(*(queue.getBackMutex()));
  sem.release(); // signal back to main thread

  queue.push(newOrder);
}