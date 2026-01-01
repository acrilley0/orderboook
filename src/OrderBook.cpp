#include <iostream>
#include "OrderBook.hpp"

void OrderBook::displayBook()
{
  std::cout << "Symbol : " << symbol << std::endl;
  std::cout << "BIDS" << std::endl;
  std::cout << "====================" << std::endl;
  for (auto it = bids.begin(); it != bids.end(); it++) {
    for (auto order : it->second) {
      std::cout << "orderId: " << order.orderId <<
        " price: " << order.price <<
        " quantity: " <<order.quantity <<
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
        " quantity: " <<order.quantity <<
        " time: " << order.timestamp << std::endl;
    }
  }
}

bool OrderBook::addOrder(Order &order)
{
  if (order.side == BID) {
    auto [iter, ins] = bids[order.price].insert(order);
    if (!ins) {
      std::cout << "Found a duplicate key, will not insert order" << std::endl;
      return false;
    }
  } else if (order.side == ASK) {
    auto [iter, ins] = asks[order.price].insert(order);
    if (!ins) {
      std::cout << "Found a duplicate key, will not insert order" << std::endl;
      return false;
    }
  }
  return true;
}
