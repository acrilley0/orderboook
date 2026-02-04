#include "OrderBook.hpp"
#include "Utils.hpp"

#include <fstream> // FIXME: For logging

std::ofstream outfile("log.txt");

OrderBook::order_execution_result_t OrderBook::addOrder(Order& order)
{
  bool inserted = false;
  std::tie(std::ignore, inserted) = global_order_index_.insert({order.order_id, order});
  if (!inserted) {
    return ORDER_REJECTED;
  }

  order_execution_result_t result = executeOrder(order);

  if (result != ORDER_FILL) {
    if (order.side == BID) {
      std::tie(std::ignore, inserted) = bids[order.price].insert(order);
    } else if (order.side == ASK) {
      std::tie(std::ignore, inserted) = asks[order.price].insert(order);
    }
  }

  return result;
}

Order* OrderBook::getOrder(const u32 order_id)
{
  auto iter = global_order_index_.find(order_id);
  if (iter == global_order_index_.end()) {
    return nullptr;
  }

  return &iter->second;
}

book_modification_result_t OrderBook::removeOrder_(const u32 order_id)
{
  Order* global_order = getOrder(order_id);
  if (global_order == nullptr) {
    return FAILURE;
  }

  if (global_order->side == BID) {
    return remove_order(order_id, global_order,  bids.at(global_order->price));
  } else {
    return remove_order(order_id, global_order, asks.at(global_order->price));
  }
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
          if (newOrder.quantity == ask.quantity) {
            result = removeOrder_(ask.order_id);
            return ORDER_FILL;
          } else if (newOrder.quantity < ask.quantity) {
            Order& askr = const_cast<Order&>(ask);
            askr.quantity -= newOrder.quantity;
            return ORDER_FILL;
          } else if (newOrder.quantity > ask.quantity) {
            newOrder.quantity -= ask.quantity;
            removeOrder_(ask.order_id);
            return ORDER_PARTIAL_FILL;
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
          if (newOrder.quantity == bid.quantity) {
            result = removeOrder_(bid.order_id);
            return ORDER_FILL;
          } else if (newOrder.quantity < bid.quantity) {
            Order& bidr = const_cast<Order&>(bid);
            bidr.quantity -= newOrder.quantity;
            return ORDER_FILL;
          } else if (newOrder.quantity > bid.quantity) {
            newOrder.quantity -= bid.quantity;
            removeOrder_(bid.order_id);
            return ORDER_PARTIAL_FILL;
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
