#pragma once
#include "types.hpp"
#include "io.hpp"

/* single outstanding limit order */
class Order {
    public:
        const t_orderid ID; 
        const t_client client;
        const t_price price;
        const SIDE side;
        t_qty qty;  // remaining
        uint8_t executionID;

    public:
        Order(const t_client, const t_orderid, const SIDE, const t_qty, const t_price);
        ~Order() = default;

        Order(const Order&) = delete; 
        Order& operator=(const Order&) = delete;
        Order(Order&& other)= delete;
        Order& operator=(Order&&) = delete;

        t_qty match(Order* const); 
        void cancel();
        bool isDone() const;
        bool canMatchPrice(const t_price) const;
};
