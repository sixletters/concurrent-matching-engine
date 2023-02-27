// This file contains declarations for the main Engine class. You will
// need to add declarations to this file as you develop your Engine.

#ifndef ENGINE_HPP
#define ENGINE_HPP

#include <chrono>
#include <atomic>
#include <unordered_map>

#include "io.hpp"
#include "orderbook.hpp"

class Engine {
  public:
	void accept(ClientConnection);
	Engine();

  private:
	std::atomic<t_client> client;
	uint32_t timestamp;
	std::unordered_map<t_orderid, Order*> allOrdersMap; 
	std::unordered_map<std::string, Orderbook*> instrumentToOrderbookMap;
	FIFOMutex engineMutex;
	void connection_thread(ClientConnection, t_client); 
};

#endif
