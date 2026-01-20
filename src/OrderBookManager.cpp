#include "OrderBookManager.hpp"
#include <iostream>

bool OrderBookManager::initBook(const std::string& symbol)
{
  if (symbol.size() == 0)
    return false;

  auto [iter, inserted] = books.emplace(symbol, std::make_unique<OrderBook>(symbol));
  return inserted;
}

OrderBook* OrderBookManager::getBook(const std::string& symbol)
{
  if (books.at(symbol) == nullptr) {
    return nullptr;
  }
  return books[symbol].get();
}

size_t OrderBookManager::getNumBooks()
{
  return books.size();
}

std::vector<std::string> OrderBookManager::getSymbols() const
{
  std::vector<std::string> symbols = {};
  for (const auto& [symbol, book] : this->books) {
    symbols.push_back(symbol);
  }

  return symbols;
}
