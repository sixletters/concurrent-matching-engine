#include <string>
#include <cstdint>
#include <sstream>
#include "order.hpp"

class SyncIO {
  public:
    std::string OrderAdded(Order* order, uint32_t timestamp) {
      std::stringstream ss
          << (order->side == SIDE::SELL ? "S " : "B " ) //
          << (order->ID) << " " //
          << (order->instrument) << " " //
          << (order->price) << " " //
          << (order->qty) << " " //
          << (timestamp); //
      return ss.str();
    }

    std::string OrderExecuted(Order* resting, Order* incoming, t_qty qty, uint32_t timestamp){
      std::stringstream ss
        << "E " //
        << (resting->ID) << " " //
        << (incoming->ID) << " " //
        << (resting->executionID) << " " //
        << (resting->price) << " " //
        << (qty) << " " //
        << (timestamp); //
      return ss.str();
    }
}