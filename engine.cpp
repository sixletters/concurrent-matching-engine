#include <iostream>
#include <thread>
#include <mutex>

#include "io.hpp"
#include "engine.hpp"

Engine::Engine() : client{0}, timestamp{0} {};


void Engine::accept(ClientConnection connection) {
	t_client clientNum = client.fetch_add(1, std::memory_order_seq_cst);
	auto th = std::thread(&Engine::connection_thread, this, std::move(connection), clientNum);
	th.detach();
}

void Engine::connection_thread(ClientConnection connection, t_client client) {
	while(true) {
		ClientCommand input {};

		{
			std::mutex readMutex; std::lock_guard<std::mutex> readLock(readMutex); 
			// local mutex won't block other threads if invalid input 
			if (connection.readInput(input) != ReadResult::Success) return; 
			// immediately lock engine on valid input
			std::lock_guard<FIFOMutex> lg(engineMutex); timestamp++;

			switch (input.type) {
				case input_cancel: { 
					// if ID not found || wrong client
					auto it = allOrdersMap.find(input.order_id);
					Order* order = it->second;
					if (it == allOrdersMap.end() || order->client != client) {
						Output::OrderDeleted(input.order_id, false, timestamp);
						break;
					};

					Orderbook* ob = instrumentToOrderbookMap[order->instrument]; 
					auto th = std::thread(&Orderbook::cancelOrder, ob, order, timestamp);
					th.detach();
					break;
				}

				case input_buy: case input_sell: {

					Order* newOrder = new Order(client, input.order_id, SIDE(input.type), input.instrument, input.count, input.price);
					// newOrder.print()
					allOrdersMap[input.order_id] = newOrder;

					auto it = instrumentToOrderbookMap.find(input.instrument);
					if (it == instrumentToOrderbookMap.end()) {
						instrumentToOrderbookMap[input.instrument] = new Orderbook(input.instrument);
						it = instrumentToOrderbookMap.find(input.instrument);
					}

					Orderbook* ob= it->second;
					auto th = std::thread(&Orderbook::createOrder, ob, newOrder, timestamp);
					th.detach();
					break;
				}
			}
		}
	}
}
