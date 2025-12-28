#include <iostream>
#include "OrderBook.hpp"

std::unique_ptr<OrderBook> OrderBook::initBook()
{
  std::unique_ptr<OrderBook> book_ptr = std::make_unique<OrderBook>(OrderBook());
  return book_ptr;
}

void OrderBook::displayBook(const std::unique_ptr<OrderBook>& book)
{
  std::cout << "BIDS" << std::endl;
  std::cout << "====================" << std::endl;
  for (auto it = book->bids.begin(); it != book->bids.end(); it++) {
    for (size_t i = 0; i < it->second.size(); i++) {
      std::cout << "orderId: " << it->second[i].orderId <<
        " price: " << it->second[i].price <<
        " quantity: " << it->second[i].quantity <<
        " time: " << it->second[i].timestamp << std::endl;
    }
  }

  std::cout << std::endl;

  std::cout << "ASKS" << std::endl;
  std::cout << "====================" << std::endl;
  for (auto it = book->asks.begin(); it != book->asks.end(); it++) {
    for (size_t i = 0; i < it->second.size(); i++) {
      std::cout << "orderId: " << it->second[i].orderId <<
        " price: " << it->second[i].price <<
        " quantity: " << it->second[i].quantity <<
        " time: " << it->second[i].timestamp << std::endl;
    }
  }
}

void OrderBook::addOrder(Order &order, std::unique_ptr<OrderBook> &book)
{
  if (order.side == BID) {
    book->bids[order.price].emplace_back(order);
  }else if (order.side == ASK) {
    book->asks[order.price].emplace_back(order);
  }
}
