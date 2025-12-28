#include <stdint.h>
#include <chrono>

typedef int32_t  i32;
typedef uint32_t u32;
typedef int64_t  i64;
typedef uint64_t u64;

typedef enum {
  BID,
  ASK,
} Side;

class Order {
public:
  u32    orderId;
  double price;
  u32    quantity;
  Side   side;
  u64    timestamp; // FIXME: Convert this to a millsecond timestamp
  // Order type?
  // Priority?

  Order(u32 orderId, double price, u32 quantity, Side side, time_t timestamp) :
    orderId(orderId),
    price(price),
    quantity(quantity),
    side(side),
    timestamp(timestamp)
  {}

  bool operator<(const Order& existing) const
  {
    if (existing.price != price)
      return existing.price < price;

    return existing.timestamp < timestamp;
  }
};
