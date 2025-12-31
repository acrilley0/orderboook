#include <stdint.h>
#include <vector>
#include <map>
#include <set>
#include <memory>
#include "Order.hpp"

typedef int32_t  i32;
typedef uint32_t u32;
typedef int64_t  i64;
typedef uint64_t u64;

class OrderBook {
public:
  // Buy Side and Sell Side, both with vectors of orders
  std::string symbol;
  std::map<double, std::set<Order>, std::greater<double>> bids;
  std::map<double, std::set<Order>, std::less<double>> asks;

  OrderBook(std::string symbol) {
    symbol = symbol;
    bids = {};
    asks = {};
  }

  OrderBook() {
    symbol = "",
    bids = {};
    asks = {};
  }

  std::unique_ptr<OrderBook> initBook(const std::string& symbol);
  std::unique_ptr<OrderBook> getBook(const std::string& symbol);
  void displayBook(const std::unique_ptr<OrderBook>& book);
  void addOrder(Order &order, std::unique_ptr<OrderBook>& book);
  void modifyOrder(OrderBook &book);
  void executeOrder(OrderBook &book);
};
