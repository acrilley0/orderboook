#include "OrderBookManager.hpp"
#include <iostream>

std::unique_ptr<OrderBook> OrderBookManager::initBook(const std::string& symbol)
{
  std::unique_ptr<OrderBook> book_ptr = std::make_unique<OrderBook>(OrderBook(symbol));
  return book_ptr;
}

OrderBook* OrderBookManager::getBook(const std::string& symbol)
{
  return books[symbol].get();
}
