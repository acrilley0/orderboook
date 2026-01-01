#include "OrderBookManager.hpp"
#include <memory>
#include <array>
#include <chrono>
#include <iostream>

enum {
  CREATE_BOOK = 1,
  ADD_ORDER,
  DISPLAY_BOOK,
};

void printOptions()
{
  std::cout << "1. Create an OrderBook" << std::endl;
  std::cout << "2. Add an Order to an OrderBook" << std::endl;
  std::cout << "3. Display an order book" << std::endl;
}

int main()
{
  OrderBookManager bookManager = OrderBookManager();

  printOptions();

  int choice;
  std::string symbol;
  while(std::cin >> choice) {
    switch (choice) {
      case CREATE_BOOK: {
        std::cout << "Please enter the symbol for the new OrderBook" << std::endl;
        std::cin.ignore();
        std::getline(std::cin, symbol);

        std::unique_ptr<OrderBook> book = bookManager.initBook(symbol);
        bookManager.books.insert({symbol, std::move(book)});
        std::cout << std::endl;
        break;
      }
      case ADD_ORDER: {
        double price;
        int    quantity;
        int    side;

        std::cout << "What OrderBook would you like to add an Order to?" << std::endl;
        std::cin.ignore();
        std::getline(std::cin ,symbol);

        OrderBook *book = bookManager.getBook(symbol);
        if (book == nullptr) {
          std::cout << "A book with symbol " << symbol << " does not currently exist!" << std::endl;
          break;
        }

        std::cout << "Enter side (1 = BUY, 2 = SELL): ";
        std::cin >> side;
        std::cout << "Enter price: ";
        std::cin >> price;
        std::cout << "Enter quantity: ";
        std::cin >> quantity;

        Order order = Order(1, quantity, price, 0, static_cast<Side>(side), symbol);

        bool added = book->addOrder(order);
        std::cout << std::endl;
        break;
      }
      case DISPLAY_BOOK: {
        std::cout << "Which symbol would you like to display the book for?" << std::endl;
        std::cin.ignore();
        std::getline(std::cin, symbol);
        OrderBook *book = bookManager.getBook(symbol);
        if (book) {
          book->displayBook();
        } else {
          std::cout << "A book for the symbol " << symbol << " does not currently exist!";
        }
        std::cout << std::endl;
        break;
      }
      default:
        std::cout << "Invalid action!" << std::endl;
        break;
    }
    printOptions();
  }

  return 0;
}
