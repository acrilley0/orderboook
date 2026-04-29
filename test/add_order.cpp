#include <gtest/gtest.h>
#include "../src/OrderBookManager.hpp"

TEST(OrderTests, AddOrderTest) {
  const std::string symbol = "AAPL";
  OrderBookManager book_manager = OrderBookManager();
  book_manager.initBook(symbol);
 
  OrderBook* book = book_manager.getBook(symbol);

  ASSERT_NE(book, nullptr);
  ASSERT_EQ(book_manager.getNumBooks(), 1);

  Order o1 = Order(1,
                   100,
                   15.00,
                   1,
                   BID,
                   symbol);
  book->addOrder(o1);

  Order o2 = Order(2,
                   100,
                   14.99,
                   1,
                   BID,
                   symbol);
  book->addOrder(o2);

  ASSERT_EQ(book->getNumOrders(BID), 2);
  ASSERT_EQ(book->bids[o1.price].size(), 1);
  ASSERT_EQ(book->bids[o2.price].size(), 1);
}

TEST(OrderTests, ExecuteOrderTest) {
  const std::string symbol = "AAPL";
  OrderBookManager book_manager = OrderBookManager();
  book_manager.initBook(symbol);
 
  OrderBook* book = book_manager.getBook(symbol);
}
