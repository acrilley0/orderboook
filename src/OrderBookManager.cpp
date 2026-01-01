#include "OrderBookManager.hpp"
#include <iostream>

std::unique_ptr<OrderBook> OrderBookManager::initBook(const std::string& symbol)
{
  std::unique_ptr<OrderBook> book_ptr = std::make_unique<OrderBook>(OrderBook(symbol));
  return book_ptr;
}

std::unique_ptr<OrderBook> OrderBookManager::getBook(const std::string& symbol, const OrderBookManager& bookManager)
{
  for (auto & iter : bookManager.books) {
    if (iter.second->symbol == symbol) {
      std::cout << "Found existing book with symbol " << iter.second->symbol << std::endl;
    }
  }

  return nullptr; // FIXME:
}
