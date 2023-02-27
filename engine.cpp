#include <iostream>
#include <thread>
#include <mutex>

#include "io.hpp"
#include "engine.hpp"

Engine::Engine() : client{0}, timestamp{0} {};


void Engine::accept(ClientConnection connection) {
	auto thread = std::thread(&Engine::connection_thread, this, std::move(connection), client++);
	thread.detach();
}

void Engine::connection_thread(ClientConnection connection, t_client client) {
	// std::mutex m;
	while(true) {
		// std::lock_guard<std::mutex> lg(m);

		ClientCommand input {};
		switch(connection.readInput(input)) {
			case ReadResult::Error: SyncCerr {} << "Error reading input" << std::endl;
			case ReadResult::EndOfFile: return;
			case ReadResult::Success: break;
		}

		
		uint32_t refTime = timestamp.fetch_add(1, std::memory_order_seq_cst);

		// Functions for printing output actions in the prescribed format are
		// provided in the Output class:
		switch(input.type) {
			case input_cancel: { 
				// if ID not found || wrong client || done
				auto it = allOrders.find(input.order_id);
				Order* order = it->second;
				if (it == allOrders.end() || order->client != client) {
					Output::OrderDeleted(input.order_id, false, refTime);
					break;
				};

				Orderbook* ob = instrumentToOrderbookMap[order->instrument]; 
				std::thread t = std::thread(&Orderbook::cancelOrder, ob, order, refTime);
				t.detach();
				break;
			}

			case input_buy: case input_sell: {

				Order* newOrder = new Order(client, input.order_id, SIDE(input.type), input.instrument, input.count, input.price);
				// newOrder.print()
				allOrders[input.order_id] = newOrder;

				auto it = instrumentToOrderbookMap.find(input.instrument);
				if (it == instrumentToOrderbookMap.end()) {
					instrumentToOrderbookMap[input.instrument] = new Orderbook(input.instrument);
					it = instrumentToOrderbookMap.find(input.instrument);
				}

				Orderbook* ob= it->second;
				std::thread t = std::thread(&Orderbook::createOrder, ob, newOrder, refTime);
				t.detach();
				break;
			}
		}
	}
}
