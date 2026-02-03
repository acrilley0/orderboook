#include <map>
#include <unordered_set>
#include <unordered_map>
#include "Order.hpp"
#include "Utils.hpp"

class OrderBook {
private:
  std::unordered_map<u32 /* orderId */, Order> global_order_index_;

  struct OrderPtrHash {
    size_t operator()(const Order& order) const {
      size_t h1 = std::hash<u32>{}(order.order_id);
      return h1;
    }
  };

  struct OrderPtrEqual {
    bool operator()(const Order& lhs, const Order& rhs) const {
      return lhs.order_id == rhs.order_id;
    }
  };

  book_modification_result_t removeOrder_(const u32 order_id);

public:
  std::string symbol;

  std::map<double, std::unordered_set<Order, OrderPtrHash, OrderPtrEqual>, std::greater<double>> bids;
  std::map<double, std::unordered_set<Order, OrderPtrHash, OrderPtrEqual>, std::less<double>> asks;

  typedef enum {
    FILL,
    PARTIAL_FILL,
  } order_execution_result_t; // FIXME: Do I actually want this to be part of the OrderBook class?

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

  void                     displayBook();
  bool                     addOrder(Order& order);
  Order*                   getOrder(u32 order_id);
  bool                     modifyOrder(Order& order);
  order_execution_result_t executeOrder(Order& newOrder);
};
