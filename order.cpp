#include <algorithm>
#include "order.hpp"
#include <stdexcept>

Order::Order( const t_client _client, const t_orderid _id, const SIDE _side, const std::string _instrument,
              const t_qty _qty, const t_price _price) : 
              ID(_id), client(_client), price(_price), side(_side), instrument(_instrument){
    qty = _qty;
    executionID = 0;
}

void Order::print() const {
    SyncCerr {}
        << "Got order: " << static_cast<char>(side) << " " << instrument << " x " << qty << " @ "
        << price << " ID: " << ID << std::endl;
}

bool Order::canMatchPrice(const t_price restingPrice) const { 
  switch (side) {
    case SIDE::BUY: return price >= restingPrice;
    case SIDE::SELL: return price <= restingPrice;
    default: throw std::runtime_error("");
  }
}
