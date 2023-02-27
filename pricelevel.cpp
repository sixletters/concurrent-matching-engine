#include <algorithm>
#include <thread>
#include <sstream>
#include "pricelevel.hpp"

PriceLevel::PriceLevel() : totalQty(0) {};

std::vector<std::string>* PriceLevel::fill(Order* const newOrder, t_qty levelFillQty, const uint32_t t) { 
  std::vector<std::string>* output = new std::vector<std::string>();
  Order* restingOrder;
  while (levelFillQty > 0) {
    restingOrder = queue.front();
    t_qty fillQty = std::min(levelFillQty, restingOrder->qty);
    levelFillQty -= fillQty;
    restingOrder->qty -= fillQty;
    restingOrder->executionID++;

    std::stringstream ss;
    ss << "E " << restingOrder->ID << " " << newOrder->ID << "" << restingOrder->executionID << " " << restingOrder->price << " " << fillQty << " " << t;
    output->push_back(ss.str());

    if (restingOrder->qty == 0) queue.pop();
  }
  queue.unlockFront();
  return output;
}

std::vector<std::string>* PriceLevel::add(Order* newOrder, const uint32_t t) { 
  std::vector<std::string>* output = new std::vector<std::string>();
  queue.push(newOrder); 

  std::stringstream ss;
  ss << (newOrder->side == SIDE::SELL ? "S " : "B " ) << newOrder->ID << " " << newOrder->instrument << " " << newOrder->price << " " << newOrder->qty << " " << t;
  output->push_back(ss.str());

  queue.unlockBack();
  return output;
}

