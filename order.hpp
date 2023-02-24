#pragma once
#include "types.hpp"

/* single outstanding limit order */
class Order {
    public:
        const t_orderid ID; 
        const t_client client;
        const t_price price;
        const t_qty _qty; 
        t_qty _leavesQty; 

        Order(const t_client, const t_orderid, const t_qty, const t_price);
        ~Order() = default;

        Order(const Order&) = delete; 
        Order& operator=(const Order&) = delete;
        Order(Order&& other)= delete;
        Order& operator=(Order&&) = delete;

        void match(Order&); 
        void cancel();
        bool isDone();
};
