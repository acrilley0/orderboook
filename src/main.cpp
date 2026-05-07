#include "UserInterface.hpp"
#include "Utils.hpp"
#include "SecurityReference.hpp"
#include <cstdlib>
#include <memory>

int main()
{
  SecurityReference::InitTable();

  OrderBookManager bookManager = OrderBookManager();

  auto screen = ftxui::ScreenInteractive::TerminalOutput();

  int current_page = static_cast<int>(Page::MAIN_MENU);
  int menu_option_selected = 0;
  std::vector<std::string> symbol_list;
  auto main_menu = UserInterface::createMainMenu(current_page, menu_option_selected);

  // Page 1: Create Book
  std::string symbol;
  modal_info_t modal_info;
  auto create_book_page = UserInterface::createBookPage(bookManager,
                                                        symbol,
                                                        symbol_list,
                                                        modal_info);

  std::vector<ftxui::Component> tabs = {main_menu, create_book_page};

  // Page 2: List Books
  auto book_list_page = UserInterface::listBooksPage(symbol_list);
  tabs.push_back(book_list_page);

  // Page 3: Add Order Page
  std::string price;
  std::string qty;

  auto add_order_page = UserInterface::addOrderPage(bookManager,
                                                    symbol,
                                                    price,
                                                    qty,
                                                    sides,
                                                    modal_info);
  tabs.push_back(add_order_page);

  // Page 5: Display Book Page
  int selected_symbol = 0;
  bool display_book_info_modal = false;
  auto display_book_page = UserInterface::displayBookPage(symbol_list,
                                                          selected_symbol,
                                                          modal_info);
  tabs.push_back(display_book_page);

  auto all_tabs = ftxui::Container::Tab(tabs, &current_page);
  auto tabs_with_modals = all_tabs;
  tabs_with_modals |= ftxui::Modal(UserInterface::createResultModal(true, "Book was created!"), &modal_info.book_success_modal_shown);
  tabs_with_modals |= ftxui::Modal(UserInterface::createResultModal(false, "Failed to create book!"), &modal_info.book_failure_modal_shown);
  tabs_with_modals |= ftxui::Modal(UserInterface::createResultModal(true, "Order was added successfully!"), &modal_info.order_add_success_modal_shown);
  tabs_with_modals |= ftxui::Modal(UserInterface::createResultModal(false, "Failed to add the order!"), &modal_info.order_add_failure_modal_shown);
  tabs_with_modals |= ftxui::Modal(UserInterface::createResultModal(true, "Your order was partially filled!"), &modal_info.order_partially_filled_modal_shown);
  tabs_with_modals |= ftxui::Modal(UserInterface::createResultModal(true, "Your order was totally filled!"), &modal_info.order_filled_modal_shown);
  tabs_with_modals |= ftxui::Modal(UserInterface::printBookInfoModal(bookManager, symbol_list, selected_symbol), &modal_info.book_info_modal_shown);

  auto final_container = ftxui::CatchEvent(tabs_with_modals, [&](ftxui::Event event) {
    if (event == ftxui::Event::Escape &&
      !modal_info.book_success_modal_shown &&
      !modal_info.book_failure_modal_shown &&
      !modal_info.order_add_success_modal_shown &&
      !modal_info.order_add_failure_modal_shown &&
      !modal_info.order_partially_filled_modal_shown &&
      !modal_info.order_filled_modal_shown &&
      !modal_info.book_info_modal_shown &&
      !display_book_info_modal) {
      current_page = static_cast<int>(Page::MAIN_MENU); // Return to main menu
      return true;
    }

    if (event == ftxui::Event::Escape && current_page == static_cast<int>(Page::MAIN_MENU)) {
      // If the user hits Escape at the main menu, it should warn them that they are about to quit
    }

    if (event == ftxui::Event::Return) {
      if (modal_info.book_success_modal_shown) {
        modal_info.book_success_modal_shown = false;
        return true;
      }
      if (modal_info.book_failure_modal_shown) {
        modal_info.book_failure_modal_shown = false;
        return true;
      }
      if (modal_info.order_add_success_modal_shown) {
        modal_info.order_add_success_modal_shown = false;
        return true;
      }
      if (modal_info.order_add_failure_modal_shown) {
        modal_info.order_add_failure_modal_shown = false;
        return true;
      }
      if (modal_info.order_partially_filled_modal_shown) {
        modal_info.order_partially_filled_modal_shown = false;
      }
      if (modal_info.order_filled_modal_shown) {
        modal_info.order_filled_modal_shown = false;
      }
      if (modal_info.book_info_modal_shown) {
        modal_info.book_info_modal_shown = false;
        return true;
      }
      return false;
    }
    return false;
  });

  screen.Loop(final_container);

  return EXIT_SUCCESS;
}
