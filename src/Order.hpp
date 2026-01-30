#include <string>
#include "Utils.hpp"

typedef enum {
  BID,
  ASK,
} Side;

class Order {
public:
  u32         orderId;
  u32         quantity;
  double      price;
  u64         timestamp;
  Side        side;
  std::string symbol;
  // Order type?
  // Priority?

  Order(u32 orderId,
        u32 quantity,
        double price,
        u64 timestamp,
        Side side,
        std::string symbol) :
    orderId(orderId),
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
