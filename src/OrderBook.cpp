#include <iostream>
#include "OrderBook.hpp"

std::unique_ptr<OrderBook> OrderBook::initBook(const std::string& symbol)
{
  std::unique_ptr<OrderBook> book_ptr = std::make_unique<OrderBook>(OrderBook(symbol));
  return book_ptr;
}

std::unique_ptr<OrderBook> getBook(const std::string& symbol)
{
  // If I am going to return a book based on a symbol, I likely need
  // a list (or some other container) of OrderBook objects
}

void OrderBook::displayBook(const std::unique_ptr<OrderBook>& book)
{
  std::cout << "BIDS" << std::endl;
  std::cout << "====================" << std::endl;
  for (auto it = book->bids.begin(); it != book->bids.end(); it++) {
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
  for (auto it = book->asks.begin(); it != book->asks.end(); it++) {
    for (auto order : it->second) {
      std::cout << "orderId: " << order.orderId <<
        " price: " << order.price <<
        " quantity: " <<order.quantity <<
        " time: " << order.timestamp << std::endl;
    }
  }
}

void OrderBook::addOrder(Order &order, std::unique_ptr<OrderBook>& book)
{
  if (order.side == BID) {
    auto [iter, ins] = book->bids[order.price].emplace(order);
    if (!ins)
      std::cout << "Found a duplicate key, will not insert order" << std::endl;
  } else if (order.side == ASK) {
    auto [iter, ins] = book->asks[order.price].emplace(order);
    if (!ins)
      std::cout << "Found a duplicate key, will not insert order" << std::endl;
  }
}
