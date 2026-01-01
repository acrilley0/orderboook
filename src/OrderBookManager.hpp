#include "OrderBook.hpp"
#include <memory>

class OrderBookManager {
  std::set<OrderBook> books; // Only one OrderBook per symbol allowed
  OrderBookManager() { books = {}; }

public:
  std::unique_ptr<OrderBook> initBook(const std::string& symbol);
  std::unique_ptr<OrderBook> getBook(const std::string& symbol);
};
