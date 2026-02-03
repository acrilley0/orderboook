#include <iostream>
#include "OrderBook.hpp"
#include "Utils.hpp"

#include <fstream> // FIXME: For logging

std::ofstream outfile("log.txt");

void OrderBook::displayBook()
{
  std::cout << "Symbol : " << symbol << std::endl;
  std::cout << "BIDS" << std::endl;
  std::cout << "====================" << std::endl;
  for (auto it = bids.begin(); it != bids.end(); it++) {
    for (auto order : it->second) {
      std::cout << "order_id: " << order.order_id <<
        " price: " << order.price <<
        " quantity: " << order.quantity <<
        " time: " << order.timestamp << std::endl;
    }
  }

  std::cout << std::endl;

  std::cout << "ASKS" << std::endl;
  std::cout << "====================" << std::endl;
  for (auto it = asks.begin(); it != asks.end(); it++) {
    for (auto order : it->second) {
      std::cout << "order_id: " << order.order_id <<
        " price: " << order.price <<
        " quantity: " << order.quantity <<
        " time: " << order.timestamp << std::endl;
    }
  }
}

bool OrderBook::addOrder(Order& order)
{
  bool inserted = false;
  std::tie(std::ignore, inserted) = global_order_index_.insert({order.order_id, order});
  if (!inserted) {
    return false;
  }

  executeOrder(order); // FIXME: Placeholder until I develop the logic here

  if (order.side == BID) {
    std::tie(std::ignore, inserted) = bids[order.price].insert(order);
  } else if (order.side == ASK) {
    std::tie(std::ignore, inserted) = asks[order.price].insert(order);
  }

  return inserted;
}

Order* OrderBook::getOrder(u32 order_id)
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

  // The key to the bids/asks maps is the price, so now I need to find by order_id
  if (global_order->side == BID) {
    auto & bids_at_price = bids.at(global_order->price);
    auto iter = bids_at_price.find(*global_order);
    if (iter != bids_at_price.end()) {
      bids_at_price.erase(*global_order);
    }
  } else if (global_order->side == ASK) {
    auto & asks_at_price = asks.at(global_order->price);
    auto iter = asks_at_price.find(*global_order);
    if (iter != asks_at_price.end()) {
      asks_at_price.erase(*global_order);
    }
  }

  global_order_index_.erase(order_id);

  return SUCCESS;
}

bool OrderBook::modifyOrder([[maybe_unused]] Order& order)
{
  return true;
}

OrderBook::order_execution_result_t OrderBook::executeOrder(Order& newOrder)
{

  switch (newOrder.side) {
    case BID: {
      // If the incoming order is a BID, we want to check if there are any matching asks
      using ask_map_t = decltype(asks)::value_type::second_type;
      ask_map_t matching_asks;
      try {
        matching_asks = asks.at(newOrder.price);
        outfile << "MATCHING ASKS" << std::endl;
        for (const auto & ask : matching_asks) {
          outfile << "id: " << ask.order_id << " price: " << ask.price << " timestamp: " << ask.timestamp << std::endl;
        }
      } catch (std::out_of_range&) {
        // No match found
      }

      break;
    }

    case ASK: {
      // If the incoming order is a ASK, we want to check if there are any matching bids
      using bid_map_t = decltype(bids)::value_type::second_type;
      bid_map_t matching_bids;
      try {
        book_modification_result_t result = SUCCESS;
        matching_bids = bids.at(newOrder.price);

        outfile << "MATCHING BIDS" << std::endl;
        for (auto & bid : matching_bids) {
          outfile << "id: " << bid.order_id << " price: " << bid.price << " timestamp: " << bid.timestamp  << std::endl;

          if (newOrder.quantity <= bid.quantity) {
            if (newOrder.quantity == bid.quantity) {
              // DELETE ORDER
              outfile << "Size before erase: " << bids.size() << std::endl;
              result = removeOrder_(bid.order_id);
              outfile << "result = " << result << " Order with ID " << bid.order_id << " has been removed from the book" << std::endl;
              outfile << "Size after erase: " << bids.size() << std::endl;
              return FILL;
            } else {
              // TODO: There is some left over quantity on the order that was already on the book
            }
          }
        }
      } catch (std::out_of_range&) {
        // No match found
      }

      break;
    }
  }

  return FILL;
}
