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

Order& OrderBook::getOrder(const u32 order_id)
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
  // FIXME: The current solution does not allow an incoming order to be matched
  // against multiple sitting orders. Is that something I want?
  switch (newOrder.side) {
    case BID: {
      // If the incoming order is a BID, we want to check if there are any matching asks
      using ask_map_t = decltype(asks)::value_type::second_type;
      try {
        book_modification_result_t result = SUCCESS;
        ask_map_t& matching_asks = asks.at(newOrder.price);

        for (auto& ask : matching_asks) {
          if (ask.quantity == newOrder.quantity) {
            removeOrder(ask.order_id, ASK);
            return ORDER_FILL;
          }
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
        book_modification_result_t result = SUCCESS;
        bid_map_t& matching_bids = bids.at(newOrder.price);

        for (auto& bid : matching_bids) {
          if (bid.price == newOrder.price) {
            removeOrder(bid.order_id, BID);
            return ORDER_FILL;
          }
        }
      } catch (std::out_of_range&) {
        // No match found
      }

      break;
    }
  }

  return ORDER_ADDED;
}
