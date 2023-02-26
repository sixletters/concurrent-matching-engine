#include <iostream>
#include <thread>
#include <atomic>

#include "io.hpp"
#include "engine.hpp"

std::atomic<uint32_t> TIMESTAMP = 0;

void Engine::accept(ClientConnection connection)
{
	auto thread = std::thread(&Engine::connection_thread, this, std::move(connection), client++);
	thread.detach();
}

void Engine::connection_thread(ClientConnection connection, t_client client)
{
	while(true)
	{
		ClientCommand input {};
		switch(connection.readInput(input))
		{
			case ReadResult::Error: SyncCerr {} << "Error reading input" << std::endl;
			case ReadResult::EndOfFile: return;
			case ReadResult::Success: break;
		}

		// Functions for printing output actions in the prescribed format are
		// provided in the Output class:
		switch(input.type)
		{
			case input_cancel: {
				// increment timestamp
				TIMESTAMP.fetch_add(1, std::memory_order_relaxed);

				// if ID not found || wrong client || done
				auto it = allOrders.find(input.order_id);
				Order* order = it->second;
				if (it == allOrders.end() || order->client != client || order->qty == 0) {
					Output::OrderDeleted(input.order_id, false, TIMESTAMP.load());
					break;
				};

				Orderbook* ob = instrumentToOrderbookMap[order->instrument];

				auto func = [](Orderbook* ob, Order* order, auto t){
					ob->cancelOrder(order, t);
				};
				auto thread = std::thread(func, ob, order, TIMESTAMP.load());
				thread.detach();
				break;
			}

			case input_buy: case input_sell: {
				// increment timestamp
				TIMESTAMP.fetch_add(1, std::memory_order_relaxed);

				Order* newOrder = new Order(client, input.order_id, SIDE(input.type), input.instrument, input.count, input.price);
				// newOrder.print()
				allOrders[input.order_id] = newOrder;

				auto it = instrumentToOrderbookMap.find(input.instrument);
				if (it == instrumentToOrderbookMap.end()) {
					instrumentToOrderbookMap[input.instrument] = new Orderbook(input.instrument);
					it = instrumentToOrderbookMap.find(input.instrument);
				}

				Orderbook* ob= it->second;
				// ob->print();
				auto func = [](Orderbook* ob, Order* newOrder, auto t){
					ob->createOrder(newOrder, t);
				};
				auto thread = std::thread(func, ob, newOrder, TIMESTAMP.load());
				thread.detach();
				break;
			}
		}
	}
}
