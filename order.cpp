#include <algorithm>
#include "order.hpp"
#include <stdexcept>

extern uint32_t TIMESTAMP;

Order::Order( const t_client _client, const t_orderid _id, const SIDE _side, const std::string _instrument,
              const t_qty _qty, const t_price _price) : 
              ID(_id), client(_client), price(_price), side(_side), instrument(_instrument){
    qty = _qty;
    executionID = 0;

    SyncCerr {}
        << "Got order: " << static_cast<char>(_side) << " " << _instrument << " x " << _qty << " @ "
        << _price << " ID: " << _id << std::endl;
}

void Order::cancel(const t_client _client) {
    std::cout<<client<<std::endl;
    std::cout<<_client<<std::endl;
  if (qty == 0 || client != _client) {
    Output::OrderDeleted(ID, false, TIMESTAMP++);
    return;
  }
  qty = 0;
  Output::OrderDeleted(ID, true, TIMESTAMP++);
}

bool Order::isDone() const { return qty == 0; }

bool Order::canMatchPrice(const t_price restingPrice) const { 
  switch (side) {
    case SIDE::BUY: return price >= restingPrice;
    case SIDE::SELL: return price <= restingPrice;
    default: throw std::runtime_error("");
  }
}
