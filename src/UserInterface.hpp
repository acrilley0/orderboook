#include <ftxui/component/component.hpp>
#include <ftxui/component/component_base.hpp>
#include <ftxui/component/component_options.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/color.hpp>
#include <ftxui/util/ref.hpp>
#include "OrderBookManager.hpp"

#define STYLE ftxui::border | ftxui::color(ftxui::Color::Blue) | ftxui::size(ftxui::WIDTH, ftxui::EQUAL, 50) | ftxui::center

enum class Action {
  CREATE_BOOK,
  LIST_BOOKS,
  ADD_ORDER,
  MODIFY_ORDER,
  DISPLAY_BOOK,
};

enum class Page {
  MAIN_MENU,
  CREATE_BOOK_PAGE,
  LIST_BOOKS_PAGE,
  ADD_ORDER_PAGE,
  MODIFY_ORDER_PAGE,
  DISPLAY_BOOK_PAGE
};

const std::vector<std::string> options = {
  "1. Create an OrderBook",
  "2. List symbols that currently have OrderBooks",
  "3. Add an Order to an OrderBook",
  "4. Modify existing order",
  "5. Display an order book",
};

class UserInterface {
public:
  static ftxui::Component createMainMenu(const OrderBookManager& bookManager,
                                         int& currentPage,
                                         int& menuOptionSelected,
                                         std::vector<std::string>& symbols);
  static ftxui::Component createBookPage(OrderBookManager& bookManager,
                                         std::string& symbol,
                                         std::vector<std::string>& symbolList,
                                         bool& inserted,
                                         bool& successModalShown,
                                         bool& failureModalShown);
  static ftxui::Component listBooksPage(std::vector<std::string>& symbols, int& selected);
  static ftxui::Component addOrderPage(OrderBook& book,
                                       std::vector<std::string>& symbolList,
                                       std::string& priceStr,
                                       std::string& quantityStr);
  static ftxui::Component createResultModal(bool result, const std::string& message);
};
