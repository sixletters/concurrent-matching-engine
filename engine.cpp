#include <iostream>
#include <thread>
#include <mutex>

#include "io.hpp"
#include "engine.hpp"

Engine::Engine() : client{0}, timestamp{0} {};


void Engine::accept(ClientConnection connection) {
	t_client clientNum = client.fetch_add(1, std::memory_order_seq_cst);
	auto thread = std::thread(&Engine::connection_thread, this, std::move(connection), clientNum);
	thread.detach();
}

void Engine::connection_thread(ClientConnection connection, t_client client) {
	while(true) {
		ClientCommand input {};

		{
			std::mutex readMutex; // local mutex won't block other threads if invalid input

			std::unique_lock<std::mutex> readLock(readMutex);
			if (connection.readInput(input) != ReadResult::Success) return;

			// lock engine on valid input
			std::lock_guard<FIFOMutex> lg(engineMutex); 
			readLock.unlock();
			uint32_t refTime = timestamp++;

			switch (input.type) {
				case input_cancel: { 
					// if ID not found || wrong client
					auto it = allOrdersMap.find(input.order_id);
					Order* order = it->second;
					if (it == allOrdersMap.end() || order->client != client) {
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
					allOrdersMap[input.order_id] = newOrder;

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
}
