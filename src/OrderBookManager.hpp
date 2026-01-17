#include "OrderBook.hpp"
#include <unordered_map>
#include <memory>
#include <vector>

class OrderBookManager {
private:
  std::unordered_map<std::string, std::unique_ptr<OrderBook>> books;

public:
  bool initBook(const std::string& symbol);
  OrderBook* getBook(const std::string& symbol);
  size_t getNumBooks();
  std::vector<std::string> getSymbols() const; // This trailing const indicates that this method will not alter the object which calls it
};
