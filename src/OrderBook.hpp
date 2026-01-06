#include <stdint.h>
#include <map>
#include <unordered_set>
#include <unordered_map>
#include "Order.hpp"

typedef int32_t  i32;
typedef uint32_t u32;
typedef int64_t  i64;
typedef uint64_t u64;

class OrderBook {
private:
  std::unordered_map<u32 /* orderId */, Order> globalOrderIndex;

  struct OrderPtrHash {
    size_t operator()(const Order& order) const {
      size_t h1 = std::hash<u32>{}(order.orderId);
      return h1;
    }
  };

  struct OrderPtrEqual {
    bool operator()(const Order& lhs, const Order& rhs) const {
      return lhs.orderId == rhs.orderId;
    }
  };

public:
  std::string symbol;

  std::map<double, std::unordered_set<Order, OrderPtrHash, OrderPtrEqual>, std::greater<double>> bids;
  std::map<double, std::unordered_set<Order, OrderPtrHash, OrderPtrEqual>, std::less<double>> asks;

  OrderBook(std::string symbol) {
    this->symbol = symbol;
    bids = {};
    asks = {};
  }

  OrderBook() {
    symbol = "";
    bids = {};
    asks = {};
  }

  void   displayBook();
  bool   addOrder(Order& order);
  Order* getOrder(u32 orderId);
  bool   modifyOrder(Order& order);
  void   executeOrder(OrderBook& book);
};
