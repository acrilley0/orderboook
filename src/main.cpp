#include "OrderBookManager.hpp"
#include <memory>
#include <array>
#include <chrono>
#include <iostream>

enum {
  CREATE_BOOK = 1,
  LIST_BOOKS,
  ADD_ORDER,
  MODIFY_ORDER,
  DISPLAY_BOOK,
};

void printOptions()
{
  std::cout << "1. Create an OrderBook" << std::endl;
  std::cout << "2. List symbols that currently have OrderBooks" << std::endl;
  std::cout << "3. Add an Order to an OrderBook" << std::endl;
  std::cout << "4. Modify existing order" << std::endl;
  std::cout << "5. Display an order book" << std::endl;
}

int main()
{
  OrderBookManager bookManager = OrderBookManager();

  printOptions();

  std::string symbol;
  u32         orderId;
  double      price;
  int         quantity;
  int         side;

  int choice;
  while(std::cin >> choice) {
    std::cout << std::endl;
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
      case LIST_BOOKS: {
        if (!bookManager.books.size())
          std::cout << "There are currently 0 orderbooks." << std::endl;

        for (auto & iter : bookManager.books) {
          std::cout << iter.second->symbol << std::endl;
        }
        break;
      }
      case ADD_ORDER: {
        std::cout << "What OrderBook would you like to add an Order to?" << std::endl;
        std::cin.ignore();
        std::getline(std::cin, symbol);

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

        const auto time = std::chrono::system_clock::now().time_since_epoch().count();

        // FIXME: Figure out how you want to generate orderIds
        Order order = Order(1, quantity, price, time, static_cast<Side>(side), symbol);

        book->addOrder(order);
        std::cout << std::endl;
        break;
      }
      case MODIFY_ORDER: {
        std::cout << "What OrderBook would you like to modify an Order in?" << std::endl;
        std::cin.ignore();
        std::getline(std::cin ,symbol);

        OrderBook *book = bookManager.getBook(symbol);
        if (book == nullptr) {
          std::cout << "A book with symbol " << symbol << " does not currently exist!" << std::endl;
          break;
        }

        std::cout << "Enter the following information for the order you want to modify:" << std::endl;
        std::cout << "Symbol: ";
        std::getline(std::cin, symbol);
        std::cout << "Order ID: ";
        std::cin >> orderId;
        std::cout << "Price: ";
        std::cin >> price;
        std::cout << "Quantity: ";
        std::cin >> quantity;
        std::cout << "Side (1 = BUY, 2 = SELL): ";
        std::cin >> side;

        Order orderToBeModified = Order(orderId, quantity, price, 0, static_cast<Side>(side), symbol);

        bool wasModified =  book->modifyOrder(orderToBeModified);

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
    std::cout << std::endl;
    printOptions();
  }

  return 0;
}
