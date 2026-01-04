#include "OrderBook.hpp"
#include <unordered_map>
#include <memory>

class OrderBookManager {

public:
  std::unordered_map<std::string, std::unique_ptr<OrderBook>> books; // Only one OrderBook per symbol allowed // FIXME: Might want to make this private and have functions for pushing new books
  bool initBook(const std::string& symbol);
  OrderBook* getBook(const std::string& symbol);
};
