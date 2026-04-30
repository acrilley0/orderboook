#include "OrderBook.hpp"
#include "Utils.hpp"

#include <fstream> // FIXME: For logging

std::ofstream outfile("log.txt");

OrderBook::order_execution_result_t OrderBook::addOrder(Order& order)
{
  order_execution_result_t result = executeOrder(order);

  if (result != ORDER_FILL) {
    if (order.side == BID) {
      bids[order.price].push_back(order);
      order_index[order.order_id] = std::prev(bids[order.price].end());
    } else if (order.side == ASK) {
      asks[order.price].push_back(order);
      order_index[order.order_id] = std::prev(asks[order.price].end());
    }
  }

  return result;
}

Order& OrderBook::getOrder(const u32 order_id) const
{
  auto iter = order_index.at(order_id);
  return *iter;
}

void OrderBook::removeOrder(const u32 order_id, Side side)
{
  auto iter = order_index.at(order_id);
  u32 price = iter->price;
  if (side == BID) {
    bids[iter->price].erase(iter);
    if (bids[price].empty()) {
      bids.erase(price);
    }
  } else {
    asks[iter->price].erase(iter);
    if (asks[price].empty()) {
      asks.erase(price);
    }
  }

  order_index.erase(order_id);
}

OrderBook::order_execution_result_t OrderBook::executeOrder(Order& newOrder)
{
  switch (newOrder.side) {
    case BID: {
      // If the incoming order is a BID, we want to check if there are any matching asks
      using ask_map_t = decltype(asks)::value_type::second_type;
      try {
        ask_map_t& matching_asks = asks.at(newOrder.price);

        for (auto iter = matching_asks.begin(); iter != matching_asks.end();) {
          if (iter->quantity == newOrder.quantity) {
            removeOrder(iter->order_id, ASK);
            return ORDER_FILL;
          } else if (iter->quantity > newOrder.quantity) {
            iter->quantity -= newOrder.quantity;
            return ORDER_FILL;
          } else if (iter->quantity < newOrder.quantity) {
            auto next = std::next(iter);
            removeOrder(iter->order_id, ASK);
            if (asks.count(newOrder.price) == 0) { break; }
            iter = next;
          }
        }

        if (newOrder.quantity > 0) {
          return ORDER_PARTIAL_FILL;
        }
      } catch (std::out_of_range&) {
        // No match found
      }

      break;
    }

    case ASK: {
      // If the incoming order is a ASK, we want to check if there are any matching bids
      using bid_map_t = decltype(bids)::value_type::second_type;
      try {
        bid_map_t& matching_bids = bids.at(newOrder.price);

        for (auto iter = matching_bids.begin(); iter != matching_bids.end();) {
          if (iter->quantity == newOrder.quantity) {
            removeOrder(iter->order_id, BID);
            return ORDER_FILL;
          } else if (iter->quantity > newOrder.quantity) {
            iter->quantity -= newOrder.quantity;
            return ORDER_FILL;
          } else if (iter->quantity < newOrder.quantity) {
            newOrder.quantity -= iter->quantity;
            auto next = std::next(iter);
            removeOrder(iter->order_id, BID);
            if (bids.count(newOrder.price) == 0) { break; }
            iter = next;
          }
        }

        if (newOrder.quantity > 0) {
          return ORDER_PARTIAL_FILL;
        }
      } catch (std::out_of_range&) {
        // No match found
      }

      break;
    }
  }

  return ORDER_ADDED;
}

u32 OrderBook::getNumOrders(const Side side) const
{
  return side == BID ? bids.size() : asks.size();
}
