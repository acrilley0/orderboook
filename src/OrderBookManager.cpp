#include "OrderBookManager.hpp"
#include <iostream>

std::unique_ptr<OrderBook> OrderBookManager::initBook(const std::string& symbol)
{
  std::unique_ptr<OrderBook> book_ptr = std::make_unique<OrderBook>(OrderBook(symbol));
  return book_ptr;
}

OrderBook* OrderBookManager::getBook(const std::string& symbol)
{
  for (auto & iter : books) {
    if (iter.second->symbol == symbol) {
      return iter.second.get();
    }
  }

  return nullptr; // FIXME:
}
