#pragma once
#include <string>
#include <vector>
#include <map>
#include <queue>
#include "order.hpp"
#include "types.hpp"

class Orderbook {
  private:
    std::map<t_price, std::queue<Order*>*> _bids;
    std::map<t_price, std::queue<Order*>*> _asks;
    std::vector<Order*> _allOrders; 

    std::map<t_price, std::queue<Order*>*>& _sameSide(const Side side);
    std::map<t_price, std::queue<Order*>*>& _oppSide(const Side side);

    static void matchOrder(std::map<t_price, std::queue<Order*>*>&, Order&);

  public:
    const std::string symbol;

    Orderbook(const std::string);
    ~Orderbook() = default; 

    Orderbook(const Orderbook&) = delete; 
    Orderbook& operator=(const Orderbook&) = delete; 
    Orderbook(Orderbook&&) = delete; 
    Orderbook& operator=(Orderbook&&) = delete; 

    void print() const;

    void createOrder(const t_client, const Side, const t_qty, const t_price);
    void cancelOrder(const t_client, const t_orderid);
};
