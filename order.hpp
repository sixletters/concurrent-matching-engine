#pragma once
#include "types.hpp"
#include "io.hpp"

/* single outstanding limit order */
struct Order {
    const t_orderid ID; 
    const t_client client;
    const t_price price;
    const SIDE side;
    const std::string instrument;
    t_qty qty;  // remaining
    uint32_t executionID;

    Order(const t_client, const t_orderid, const SIDE, const std::string instrument, const t_qty, const t_price);
    ~Order() = default;

    Order(const Order&) = delete; 
    Order& operator=(const Order&) = delete;
    Order(Order&&)= delete;
    Order& operator=(Order&&) = delete;

    void print() const;
    bool canMatchPrice(const t_price) const;
};
