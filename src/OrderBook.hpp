#pragma once

#include <map>
#include <unordered_map>
#include "Order.hpp"
#include "SecurityReference.hpp"
#include "Utils.hpp"
#include <list>

class OrderBook {
private:
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

  bool                       modifyOrder_(const u32 order_id);
  book_modification_result_t removeOrder_(const u32 order_id);

public:
  std::string symbol;
  SecurityReference sec_ref_data;

  std::map<f64, std::list<Order>, std::greater<double>> bids;
  std::map<f64, std::list<Order>, std::less<double>> asks;
  std::unordered_map<u32, std::list<Order>::iterator> order_index;

  typedef enum {
    ORDER_ADDED,
    ORDER_REJECTED,
    ORDER_FILL,
    ORDER_PARTIAL_FILL,
  } order_execution_result_t; // FIXME: Do I actually want this to be part of the OrderBook class?

  OrderBook(std::string_view symbol) {
    this->symbol = symbol;
    bids = {};
    asks = {};
  }

  OrderBook() {
    symbol = "";
    bids = {};
    asks = {};
  }
  order_execution_result_t addOrder(Order& order);
  Order&                   getOrder(const u32 order_id) const;
  order_execution_result_t executeOrder(Order& new_order);
  void                     removeOrder(const u32 order_id, Side side);
  u32                      getNumOrders(const Side side) const;
};
