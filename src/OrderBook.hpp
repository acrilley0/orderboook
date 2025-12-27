#include <stdint.h>
#include <vector>
#include <memory>
#include "Order.hpp"

typedef int32_t  i32;
typedef uint32_t u32;
typedef int64_t  i64;
typedef uint64_t u64;

struct Level {
  u32 levelId;
  std::vector<Order> orders;

  Level(u32 levelId, std::vector<Order> orders) :
    levelId(levelId),
    orders(orders)
  {}
};

class OrderBook {
public:
  // Buy Side and Sell Side, both with vectors of orders
  std::vector<Level> bids;
  std::vector<Level> asks;

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
