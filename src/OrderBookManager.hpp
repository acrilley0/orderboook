#pragma once

#include "OrderBook.hpp"
#include <unordered_map>
#include <memory>
#include <vector>

class OrderBookManager {
private:
  std::unordered_map<std::string, std::unique_ptr<OrderBook>> books;

public:
  bool initBook(const std::string& symbol);
  OrderBook* getBook(const std::string& symbol) const;
  size_t getNumBooks() const;
  std::vector<std::string> getSymbols() const;
};
