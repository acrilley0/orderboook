#include "OrderBookManager.hpp"
#include <memory>
#include <chrono>
#include <iostream>

int main()
{
  OrderBookManager bookManager = OrderBookManager();
  std::unique_ptr<OrderBook> aaplBook = bookManager.initBook("AAPL");
  std::unique_ptr<OrderBook> tslaBook = bookManager.initBook("TSLA");
  // NOTE: We have to call std::move on the unique_ptr to the OrderBook because
  // you can't insert a unique_ptr into a map directly. This is because a unique_ptr
  // cannot be copied, only moved, and the insert method of the unordered_map tries
  // to copy the unique_ptr
  bookManager.books.insert({"AAPL", std::move(aaplBook)});
  bookManager.books.insert({"TSLA", std::move(tslaBook)});


  OrderBookManager::getBook("AAPL", bookManager);
  OrderBookManager::getBook("TSLA", bookManager);

  return 0;
}
