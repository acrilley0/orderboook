#pragma once

#include <string>
#include "Utils.hpp"

typedef enum {
  BID,
  ASK,
} Side;

class Order {
public:
  u32          order_id;
  u32          quantity;
  f64          price;
  u64          timestamp;
  Side         side;
  std::string symbol;
  // Priority?

  Order(u32 order_id,
        u32 quantity,
        f64 price,
        u64 timestamp,
        Side side,
        std::string symbol) :
    order_id(order_id),
    quantity(quantity),
    price(price),
    timestamp(timestamp),
    side(side),
    symbol(symbol)
  {}

  bool operator<(const Order& existing) const
  {
    if (existing.price != price)
      return existing.price < price;

    return existing.timestamp < timestamp;
  }
};
