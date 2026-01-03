#include <stdint.h>
#include <map>
#include <unordered_set>
#include <unordered_map>
#include <memory>
#include "Order.hpp"

typedef int32_t  i32;
typedef uint32_t u32;
typedef int64_t  i64;
typedef uint64_t u64;

class OrderBook {
private:
  struct OrderPtrHash {
    size_t operator()(const std::shared_ptr<Order>& order) const {
      size_t h1 = std::hash<u32>{}(order->orderId);
      // size_t h2 = std::hash<std::string>{}(order.symbol);
      // size_t h3 = std::hash<bool>{}(order.side);

      /* An order's uniqueness is a combination of it's
       * ID, the symbol it corresponds to, and its side
       */
      // return h1 ^ (h2 << 1) ^ (h3 << 2);
      return h1;
    }
  };

  struct OrderPtrEqual {
    bool operator()(const std::shared_ptr<Order>& lhs, const std::shared_ptr<Order>& rhs) const {
      return lhs->orderId == rhs->orderId;
    }
  };

public:
  std::string symbol;

  std::unordered_map<u32 /* orderId */, std::shared_ptr<Order>> globalOrderIndex;

  std::map<double, std::unordered_set<std::shared_ptr<Order>, OrderPtrHash, OrderPtrEqual>, std::greater<double>> bids;
  std::map<double, std::unordered_set<std::shared_ptr<Order>, OrderPtrHash, OrderPtrEqual>, std::less<double>> asks;

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
  bool   addOrder(std::shared_ptr<Order>& order);
  Order* getOrder(u32 orderId);
  bool   modifyOrder(Order &order);
  void   executeOrder(OrderBook &book);
};
