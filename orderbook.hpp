#pragma once
#include <string>
#include <map>
#include <unordered_map>
#include "fifomutex.hpp"
#include "order.hpp"
#include "types.hpp"
#include "pricelevel.hpp"

struct PriceComp {
  bool ascending;
  PriceComp(bool _ascending) : ascending(_ascending) {};
  bool operator()(t_price lhs, t_price rhs) const {
    return ascending ? lhs < rhs : lhs > rhs;
  };
};

using PRICELEVELMAP = std::map<t_price, PriceLevel*, PriceComp>;

class Orderbook {
  private:
    PRICELEVELMAP _bids;
    PRICELEVELMAP _asks;

    PRICELEVELMAP& _sameSide(const SIDE side);
    PRICELEVELMAP& _oppSide(const SIDE side);

  public:
    FIFOMutex orderbookLock;
    const std::string instrument;

    explicit Orderbook(const std::string);
    ~Orderbook() = default; 

    Orderbook(const Orderbook&) = delete; 
    Orderbook& operator=(const Orderbook&) = delete; 
    Orderbook(Orderbook&&) = delete; 
    Orderbook& operator=(Orderbook&&) = delete; 

    void print() const;

    void createOrder(Order* const, uint32_t);
    void cancelOrder(Order* const, uint32_t);
};
