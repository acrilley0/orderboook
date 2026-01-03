#include <iostream>
#include "OrderBook.hpp"

void OrderBook::displayBook()
{
  std::cout << "Symbol : " << symbol << std::endl;
  std::cout << "BIDS" << std::endl;
  std::cout << "====================" << std::endl;
  for (auto it = bids.begin(); it != bids.end(); it++) {
    for (auto order : it->second) {
      std::cout << "orderId: " << order->orderId <<
        " price: " << order->price <<
        " quantity: " << order->quantity <<
        " time: " << order->timestamp << std::endl;
    }
  }

  std::cout << std::endl;

  std::cout << "ASKS" << std::endl;
  std::cout << "====================" << std::endl;
  for (auto it = asks.begin(); it != asks.end(); it++) {
    for (auto order : it->second) {
      std::cout << "orderId: " << order->orderId <<
        " price: " << order->price <<
        " quantity: " << order->quantity <<
        " time: " << order->timestamp << std::endl;
    }
  }
}

bool OrderBook::addOrder(std::shared_ptr<Order>& order)
{
  bool inserted = false;
  std::tie(std::ignore, inserted) = globalOrderIndex.insert({order->orderId, order});
  if (!inserted) {
    return false;
  }

  if (order->side == BID) {
    std::tie(std::ignore, inserted) = bids[order->price].insert(order);
  } else if (order->side == ASK) {
    std::tie(std::ignore, inserted) = asks[order->price].insert(order);
  }

  if (!inserted) {
    std::cout << "Failed to insert order!" << std::endl;
    return false;
  }

  std::cout << "Order was added successfully!" << std::endl;

  return true;
}

template<typename MapType>
Order* findOrderInMap(MapType& orderMap, u32 orderId) {
  auto iter = orderMap.find(orderId);
  if (iter == orderMap.end()) {
    return nullptr;
  }

  return iter->second.get();
}

Order* OrderBook::getOrder(u32 orderId)
{
  return findOrderInMap(globalOrderIndex, orderId);
}

bool OrderBook::modifyOrder(Order &order)
{
}
