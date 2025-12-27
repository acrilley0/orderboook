#include <iostream>
#include "OrderBook.hpp"

std::unique_ptr<OrderBook> OrderBook::initBook()
{
  std::unique_ptr<OrderBook> book_ptr = std::make_unique<OrderBook>(OrderBook());
  return book_ptr;
}

void OrderBook::displayBook(const std::unique_ptr<OrderBook>& book)
{
  std::cout << "BIDS" << std::endl;
  std::cout << "====================" << std::endl;
  for (Level lvl : book->bids) {
    for (Order ord : lvl.orders) {
      std::cout << "orderId: " << ord.orderId << " price: " << ord.price << " quantity: " << ord.quantity << " time: " << ord.timestamp << std::endl;
    }
  }

  std::cout << std::endl;

  std::cout << "ASKS" << std::endl;
  std::cout << "====================" << std::endl;
  for (Level lvl : book->asks) {
    for (Order ord : lvl.orders) {
      std::cout << "orderId: " << ord.orderId << " price: " << ord.price << " quantity: " << ord.quantity << " time: " << ord.timestamp << std::endl;
    }
  }
}

void OrderBook::addOrder(Order &order, std::unique_ptr<OrderBook> &book)
{
  if (order.side == BID) {
    // book->bids[0].orders.emplace_back(order);
    Level lvl = Level(1, {order});
    book->bids.emplace_back(lvl);
  }else if (order.side == ASK) {
    // book->asks[0].orders.emplace_back(order);
  }
}
