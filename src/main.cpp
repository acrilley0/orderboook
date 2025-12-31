#include "OrderBook.hpp"
#include <memory>
#include <chrono>

int main()
{
  OrderBook ob;
  std::unique_ptr<OrderBook> book = ob.initBook("AAPL");

  auto current_time = std::chrono::system_clock::now();
  auto c_time = std::chrono::system_clock::to_time_t(current_time);
  Order ord1("AAPL", 1, 50.25, 100, BID, c_time);
  ob.addOrder(ord1, book);

  current_time = std::chrono::system_clock::now();
  c_time = std::chrono::system_clock::to_time_t(current_time);
  Order ord2("TSLA", 2, 50.50, 125, BID, c_time);
  ob.addOrder(ord2, book);

  // current_time = std::chrono::system_clock::now();
  // c_time = std::chrono::system_clock::to_time_t(current_time);
  // Order ord3(3, 50.75, 150, BID, c_time);
  // ob.addOrder(ord3, book);
  //
  // current_time = std::chrono::system_clock::now();
  // c_time = std::chrono::system_clock::to_time_t(current_time);
  // Order ord4(1, 50.80, 125, ASK, c_time);
  // ob.addOrder(ord4, book);
  //
  // current_time = std::chrono::system_clock::now();
  // c_time = std::chrono::system_clock::to_time_t(current_time);
  // Order ord5(2, 50.70, 110, ASK, c_time);
  // ob.addOrder(ord5, book);
  //
  // current_time = std::chrono::system_clock::now();
  // c_time = std::chrono::system_clock::to_time_t(current_time);
  // Order ord6(3, 50.65, 130, ASK, c_time);
  // ob.addOrder(ord6, book);
  //
  // current_time = std::chrono::system_clock::now();
  // c_time = std::chrono::system_clock::to_time_t(current_time);
  // Order ord7(4, 50.55, 100, ASK, c_time);
  // ob.addOrder(ord7, book);
  //
  // current_time = std::chrono::system_clock::now();
  // c_time = std::chrono::system_clock::to_time_t(current_time);
  // Order ord8(4, 50.55, 100, ASK, c_time);
  // ob.addOrder(ord8, book);
  //
  // current_time = std::chrono::system_clock::now();
  // c_time = std::chrono::system_clock::to_time_t(current_time);
  // Order ord9(4, 50.55, 100, ASK, c_time);
  // ob.addOrder(ord9, book);

  ob.displayBook(book);

  return 0;
}
