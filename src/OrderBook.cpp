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
  std::cout << "Order was successfully added!" << std::endl;

  return true;
}

template<typename MapType>
Order* findOrderInMap(MapType& orderMap, u32 orderId, double price, const std::string& symbol, Side side) {
    auto iter = orderMap.find(price);
    if (iter == orderMap.end()) {
        return nullptr;
    }

    auto& ordersAtPrice = iter->second;
    auto orderIter = ordersAtPrice.find(Order(orderId, 0, price, 0, side, symbol));
    if (orderIter != ordersAtPrice.end()) {
        return const_cast<Order*>(&(*orderIter));
    }
    return nullptr;
}

bool OrderBook::modifyOrder(Order &order)
{
  Order *orderInBook = nullptr;
  if (order.side == BID) {
    orderInBook = findOrderInMap(this->bids, order.orderId, order.price, order.symbol, order.side);
  } else if (order.side == ASK) {
    orderInBook = findOrderInMap(this->asks, order.orderId, order.price, order.symbol, order.side);
  }

  return orderInBook;
}
