#include <ftxui/component/component.hpp>
#include <ftxui/component/component_base.hpp>
#include <ftxui/component/component_options.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/color.hpp>
#include <ftxui/util/ref.hpp>
#include "OrderBookManager.hpp"
#include "Utils.hpp"

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
  DISPLAY_BOOK_PAGE,
  MODIFY_ORDER_PAGE,
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
  static ftxui::Component createMainMenu(int& current_page,
                                         int& menu_option_selected);
  static ftxui::Component createBookPage(OrderBookManager& book_manager,
                                         std::string& symbol,
                                         std::vector<std::string>& symbol_list,
                                         modal_info_t& modal_info);
  static ftxui::Component listBooksPage(std::vector<std::string>& symbols);
  static ftxui::Component addOrderPage(OrderBookManager& book_manager,
                                       std::string& symbol,
                                       std::string& price_str,
                                       std::string& quantity_str,
                                       const std::vector<std::string>& sides,
                                       modal_info_t& modal_info);
  static ftxui::Component printBookInfoModal(OrderBookManager& book_manager,
                                             const std::vector<std::string>& symbol_list,
                                             int& symbol_index);
  static ftxui::Component displayBookPage(std::vector<std::string>& symbol_list,
                                          int& selected_symbol,
                                          modal_info_t& modal_info);
  static ftxui::Component createResultModal(bool result, const std::string& message);
};
