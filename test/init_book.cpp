#include <gtest/gtest.h>
#include "../src/OrderBookManager.hpp"

TEST(BookTests, InitBookTest) {
  OrderBookManager bookManager = OrderBookManager();
  const std::string aapl = "AAPL";

  bool aapl_book_created = bookManager.initBook(aapl);
  ASSERT_EQ(aapl_book_created, true);

  // This should fail because a book with symbol "AAPL" already exists
  bool second_aapl_book_created = bookManager.initBook(aapl);
  ASSERT_EQ(second_aapl_book_created, false);

  // This should fail because the trimming function will transform
  // "  AAPL  " into "AAPL", which we already have a book for
  bool tsla_book_created = bookManager.initBook("TSLA");
  ASSERT_EQ(tsla_book_created, true);
}
