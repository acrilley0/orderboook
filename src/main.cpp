#include "OrderBookManager.hpp"
#include <memory>
#include <array>
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
  // NOTE: std::move transfers the ownership of the unique_ptr from aaplBook
  // and tslaBook to the bookManager.books["AAPL"] and bookManager.books["TSLA"]
  // so to access them from this point on we can not simply use aaplBook and
  // tslaBook anymore

  std::array<Order, 8> orders = {
    Order(1, 100, 25.00, 0, BID, "AAPL"),
    Order(2, 120, 25.20, 0, BID, "AAPL"),
    Order(3, 130, 25.25, 0, BID, "AAPL"),
    Order(4, 140, 25.30, 0, BID, "AAPL"),
    Order(1, 100, 25.00, 0, ASK, "AAPL"),
    Order(2, 120, 25.20, 0, ASK, "AAPL"),
    Order(3, 130, 25.25, 0, ASK, "AAPL"),
    Order(4, 140, 25.30, 0, ASK, "AAPL"),
  };

  for (Order ord : orders) {
    bookManager.books["AAPL"]->addOrder(ord);
  }

  bookManager.books["AAPL"]->displayBook();

  bookManager.getBook("AAPL");
  bookManager.getBook("TSLA");

  return 0;
}
