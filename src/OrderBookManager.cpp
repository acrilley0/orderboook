#include "OrderBookManager.hpp"
#include <iostream>

// OrderBook* OrderBookManager::initBook(const std::string& symbol)
bool OrderBookManager::initBook(const std::string& symbol)
{
  // std::unique_ptr<OrderBook> book_ptr = std::make_unique<OrderBook>(OrderBook(symbol));
  // return book_ptr;
  auto [iter, inserted] = books.emplace(symbol, std::make_unique<OrderBook>(symbol));
  // return iter->second.get();
  return inserted;
}

OrderBook* OrderBookManager::getBook(const std::string& symbol)
{
  return books[symbol].get();
}
