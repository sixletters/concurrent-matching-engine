#pragma once
#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <mutex>
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

using PL_MAP = std::map<t_price, PriceLevel*, PriceComp>;

class Orderbook {
  private:
    PL_MAP _bids;
    PL_MAP _asks;

    PL_MAP& _sameSide(const SIDE side);
    PL_MAP& _oppSide(const SIDE side);

  public:
    std::mutex orderbookLock;
    const std::string instrument;

    explicit Orderbook(const std::string);
    ~Orderbook() = default; 

    Orderbook(const Orderbook&) = delete; 
    Orderbook& operator=(const Orderbook&) = delete; 
    Orderbook(Orderbook&&) = delete; 
    Orderbook& operator=(Orderbook&&) = delete; 

    void print() const;

    void createOrder(Order* const);
    void cancelOrder(Order* const, t_client client);
};
