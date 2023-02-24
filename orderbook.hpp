#pragma once
#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include "order.hpp"
#include "types.hpp"
#include "pricelevel.hpp"

class Orderbook {
  private:
    std::map<t_price, PriceLevel*> _bids;
    std::map<t_price, PriceLevel*> _asks;
    std::unordered_map<t_orderid, Order*> _allOrders; 

    std::map<t_price, PriceLevel*>& _sameSide(const Side side);
    std::map<t_price, PriceLevel*>& _oppSide(const Side side);

    static void matchOrder(std::map<t_price, PriceLevel*>&, Order*);

  public:
    const std::string symbol;

    Orderbook(const std::string);
    ~Orderbook() = default; 

    Orderbook(const Orderbook&) = delete; 
    Orderbook& operator=(const Orderbook&) = delete; 
    Orderbook(Orderbook&&) = delete; 
    Orderbook& operator=(Orderbook&&) = delete; 

    void print() const;

    void createOrder(const t_client, const t_orderid, const Side, const t_qty, const t_price);
    void cancelOrder(const t_client, const t_orderid);
};
