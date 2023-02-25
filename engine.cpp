#include <iostream>
#include <thread>

#include "io.hpp"
#include "engine.hpp"

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
				SyncCerr {} << "Got cancel: ID: " << input.order_id << std::endl;

				auto it = allOrders.find(input.order_id);
				if (it == allOrders.end()) {
					Output::OrderDeleted(input.order_id, false, 420);
					break;
				};

				Order* order = it->second;
				order->cancel(client);
				break;
			}

			case input_buy: case input_sell: {
				SyncCerr {}
				    << "Got order: " << static_cast<char>(input.type) << " " << input.instrument << " x " << input.count << " @ "
				    << input.price << " ID: " << input.order_id << std::endl;

				Order* newOrder = new Order(client, input.order_id, SIDE(input.type), input.count, input.price);
				allOrders[input.order_id] = newOrder;

				auto it = instrumentToOrderbookMap.find(input.instrument);
				if (it == instrumentToOrderbookMap.end()) {
					instrumentToOrderbookMap[input.instrument] = new Orderbook(input.instrument);
					it = instrumentToOrderbookMap.find(input.instrument);
				}

				Orderbook* ob= it->second;
				ob->print();
				auto func = [](Orderbook* ob, Order* newOrder){
					ob->createOrder(newOrder);
				};
				auto thread = std::thread(func, ob, client, input);
				thread.detach();
				break;
			}
		}
	}
}
