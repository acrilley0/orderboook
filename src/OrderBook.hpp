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

struct Level {
  u32 levelId;
  std::set<Order> orders;

  Level(u32 levelId, std::set<Order> orders) :
    levelId(levelId),
    orders(orders)
  {}
};

class OrderBook {
public:
  // Buy Side and Sell Side, both with vectors of orders
  std::map<double, std::vector<Order>, std::greater<double>> bids;
  std::map<double, std::vector<Order>, std::less<double>> asks;

  OrderBook() {
    bids = {};
    asks = {};
  }

  std::unique_ptr<OrderBook> initBook();
  void displayBook(const std::unique_ptr<OrderBook>& book);
  void addOrder(Order &order, std::unique_ptr<OrderBook> &book);
  void modifyOrder(OrderBook &book);
  void executeOrder(OrderBook &book);
};
