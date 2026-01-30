#include <iostream>
#include "OrderBook.hpp"

#include <fstream> // FIXME: For logging
#include <stdexcept>

void OrderBook::displayBook()
{
  std::cout << "Symbol : " << symbol << std::endl;
  std::cout << "BIDS" << std::endl;
  std::cout << "====================" << std::endl;
  for (auto it = bids.begin(); it != bids.end(); it++) {
    for (auto order : it->second) {
      std::cout << "orderId: " << order.orderId <<
        " price: " << order.price <<
        " quantity: " << order.quantity <<
        " time: " << order.timestamp << std::endl;
    }
  }

  std::cout << std::endl;

  std::cout << "ASKS" << std::endl;
  std::cout << "====================" << std::endl;
  for (auto it = asks.begin(); it != asks.end(); it++) {
    for (auto order : it->second) {
      std::cout << "orderId: " << order.orderId <<
        " price: " << order.price <<
        " quantity: " << order.quantity <<
        " time: " << order.timestamp << std::endl;
    }
  }
}

bool OrderBook::addOrder(Order& order)
{
  bool inserted = false;
  std::tie(std::ignore, inserted) = globalOrderIndex.insert({order.orderId, order});
  if (!inserted) {
    return false;
  }

  executeOrder(order); // FIXME: Placeholder until I develop the logic here

  if (order.side == BID) {
    std::tie(std::ignore, inserted) = bids[order.price].insert(order);
  } else if (order.side == ASK) {
    std::tie(std::ignore, inserted) = asks[order.price].insert(order);
  }

  return inserted;
}

Order* OrderBook::getOrder(u32 orderId)
{
  auto iter = globalOrderIndex.find(orderId);
  if (iter == globalOrderIndex.end()) {
    return nullptr;
  }

  return &iter->second;
}

bool OrderBook::modifyOrder(Order& order)
{
}

void OrderBook::executeOrder(Order& newOrder)
{
  // std::ofstream outfile("log.txt");

  switch (newOrder.side) {
    case BID: {
      // If the incoming order is a BID, we want to check if there are any matching asks
      using ask_map_t = decltype(asks)::value_type::second_type;
      ask_map_t matching_asks;
      try {
        matching_asks = asks.at(newOrder.price);
        for (const auto & ask : matching_asks) {
          // outfile << "id: " << ask.orderId << " price: " << ask.price << std::endl;
        }
      } catch (std::out_of_range&) {
        // No match found
      }

      if (matching_asks.size()) {
      }
      break;
    }
    case ASK: {
      break;
    }
  }

  // outfile.close();
}
