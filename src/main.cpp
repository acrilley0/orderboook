#include "UserInterface.hpp"
#include "Utils.hpp"
#include <cstdlib>
#include <memory>

int main()
{
  OrderBookManager bookManager = OrderBookManager();

  auto screen = ftxui::ScreenInteractive::TerminalOutput();

  int currentPage = static_cast<int>(Page::MAIN_MENU);
  int menuOptionSelected = 0;
  std::vector<std::string> symbolList;
  auto mainMenu = UserInterface::createMainMenu(currentPage, menuOptionSelected);

  // Page 1: Create Book
  std::string symbol;
  modal_info_t modal_info;
  auto createBookPage = UserInterface::createBookPage(bookManager,
                                                      symbol,
                                                      symbolList,
                                                      modal_info);

  std::vector<ftxui::Component> tabs = {mainMenu, createBookPage};

  // Page 2: List Books
  auto bookListPage = UserInterface::listBooksPage(symbolList);
  tabs.push_back(bookListPage);

  // Page 3: Add Order Page
  std::string price;
  std::string qty;

  auto addOrderPage = UserInterface::addOrderPage(bookManager,
                                                  symbol,
                                                  price,
                                                  qty,
                                                  sides,
                                                  modal_info);
  tabs.push_back(addOrderPage);

  // Page 5: Display Book Page
  int selectedSymbol = 0;
  bool displayBookInfoModal = false;
  std::string currentSymbolForModal;
  auto displayBookPage = UserInterface::displayBookPage(symbolList,
                                                        selectedSymbol,
                                                        displayBookInfoModal,
                                                        currentSymbolForModal); // FIXME: Refactor this to use modal_info_t
  tabs.push_back(displayBookPage);

  auto allTabs = ftxui::Container::Tab(tabs, &currentPage);
  auto withModals = allTabs;
  // FIXME: The wrong text is getting shown for the book creation modal, i.e. "Order was successfully added"
  // is being shown when a book is created successfully. This started happening after you began to use the
  // modal_info_t struct
  withModals |= ftxui::Modal(UserInterface::createResultModal(true, "Book was created!"), &modal_info.book_success_modal_shown);
  withModals |= ftxui::Modal(UserInterface::createResultModal(false, "Failed to create book!"), &modal_info.book_failure_modal_shown);
  withModals |= ftxui::Modal(UserInterface::createResultModal(true, "Order was added successfully!"), &modal_info.order_success_modal_shown);
  withModals |= ftxui::Modal(UserInterface::createResultModal(false, "Failed to add the order!"), &modal_info.order_failure_modal_shown);
  withModals |= ftxui::Modal(UserInterface::printBookInfoModal(bookManager, symbolList, selectedSymbol), &displayBookInfoModal);

  auto finalContainer = ftxui::CatchEvent(withModals, [&](ftxui::Event event) {
    if (event == ftxui::Event::Escape &&
        !modal_info.book_success_modal_shown &&
        !modal_info.book_failure_modal_shown &&
        !modal_info.order_success_modal_shown &&
        !modal_info.order_failure_modal_shown &&
        !displayBookInfoModal) {
      currentPage = static_cast<int>(Page::MAIN_MENU); // Return to main menu
      return true;
    }

    if (event == ftxui::Event::Escape && currentPage == static_cast<int>(Page::MAIN_MENU)) {
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
      if (modal_info.order_success_modal_shown) {
        modal_info.order_success_modal_shown = false;
        return true;
      }
      if (modal_info.order_failure_modal_shown) {
        modal_info.order_failure_modal_shown = false;
        return true;
      }
      if (displayBookInfoModal) {
        displayBookInfoModal = false;
        return true;
      }
      return false;
    }
    return false;
  });

  screen.Loop(finalContainer);

  return EXIT_SUCCESS;
}
