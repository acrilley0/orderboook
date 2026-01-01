#include "OrderBookManager.hpp"

std::unique_ptr<OrderBook> OrderBookManager::initBook(const std::string& symbol)
{
  std::unique_ptr<OrderBook> book_ptr = std::make_unique<OrderBook>(OrderBook(symbol));
  return book_ptr;
}

std::unique_ptr<OrderBook> getBook(const std::string& symbol)
{
  // If I am going to return a book based on a symbol, I likely need
  // a list (or some other container) of OrderBook objects
}
