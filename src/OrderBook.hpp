#include <stdint.h>
#include <map>
#include <unordered_set>
#include <memory>
#include "Order.hpp"

typedef int32_t  i32;
typedef uint32_t u32;
typedef int64_t  i64;
typedef uint64_t u64;

class OrderBook {
private:
  struct OrderHash {
    size_t operator()(const Order& order) const {
      size_t h1 = std::hash<u32>{}(order.orderId);
      size_t h2 = std::hash<std::string>{}(order.symbol);
      size_t h3 = std::hash<double>{}(order.price);

      /* An order's uniqueness is a combination of it's
       * ID, the symbol it corresponds to, and its price
       */
      return h1 ^ (h2 << 1) ^ (h3 << 2);
    }
  };

  struct OrderEqual {
    bool operator()(const Order& lhs, const Order& rhs) const {
      return lhs.orderId == rhs.orderId;
    }
  };

public:
  // Buy Side and Sell Side, both with vectors of orders
  std::string symbol;
  std::map<double, std::unordered_set<Order, OrderHash, OrderEqual>, std::greater<double>> bids;
  std::map<double, std::unordered_set<Order, OrderHash, OrderEqual>, std::less<double>> asks;

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
