#include <stdint.h>
#include <string>

typedef int32_t  i32;
typedef uint32_t u32;
typedef int64_t  i64;
typedef uint64_t u64;

typedef enum {
  BID = 1,
  ASK,
} Side;

class Order {
public:
  u32         orderId;
  u32         quantity;
  double      price;
  u64         timestamp; // FIXME: Convert this to a millsecond timestamp
  Side        side;
  std::string symbol;
  // Order type?
  // Priority?

  Order(u32 orderId,
        u32 quantity,
        double price,
        time_t timestamp,
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
