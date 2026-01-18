#include "UserInterface.hpp"
#include <cstdlib>
#include <memory>
#include <chrono>
#include <iostream>

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
  bool inserted = false;
  bool bookCreationSuccessModal = false;
  bool bookCreationFailureModal = false;
  auto createBookPage = UserInterface::createBookPage(bookManager, symbol, symbolList, inserted, bookCreationSuccessModal, bookCreationFailureModal);

  std::vector<ftxui::Component> tabs = {mainMenu, createBookPage};

  // Page 2: List Books
  int selected = 0;
  auto bookListPage = UserInterface::listBooksPage(symbolList, selected);
  tabs.push_back(bookListPage);

  // Page 3: Add Order Page
  std::string price;
  std::string qty;
  const std::vector<std::string> sides = {
    "BID",
    "ASK",
  };
  int selectedSide = 0;
  bool orderAddSuccessModal = false;
  bool orderAddFailureModal = false;

  auto addOrderPage = UserInterface::addOrderPage(bookManager,
                                                  symbol,
                                                  price,
                                                  qty,
                                                  sides,
                                                  selectedSide,
                                                  orderAddSuccessModal,
                                                  orderAddFailureModal);
  tabs.push_back(addOrderPage);

  // Page 5: Display Book Page
  int selectedSymbol = 0;
  bool displayBookInfoModal = false;
  auto displayBookPage = UserInterface::displayBookPage(bookManager,
                                                        symbolList,
                                                        selectedSymbol,
                                                        displayBookInfoModal);
  tabs.push_back(displayBookPage);

  auto allTabs = ftxui::Container::Tab(tabs, &currentPage);
  auto withModals = allTabs;
  withModals |= ftxui::Modal(UserInterface::createResultModal(true, "Book was created!"), &bookCreationSuccessModal);
  withModals |= ftxui::Modal(UserInterface::createResultModal(false, "Failed to create book!"), &bookCreationFailureModal);
  withModals |= ftxui::Modal(UserInterface::createResultModal(true, "Order was added successfully!"), &orderAddSuccessModal);
  withModals |= ftxui::Modal(UserInterface::createResultModal(false, "Failed to add the order!"), &orderAddFailureModal);
  withModals |= ftxui::Modal(UserInterface::printBookInfoModal(symbolList, selectedSymbol), &displayBookInfoModal); // FIXME: This should look up a book

  auto finalContainer = ftxui::CatchEvent(withModals, [&](ftxui::Event event) {
    if (event == ftxui::Event::Escape &&
        !bookCreationSuccessModal &&
        !bookCreationFailureModal &&
        !orderAddSuccessModal &&
        !orderAddFailureModal &&
        !displayBookInfoModal) {
      currentPage = static_cast<int>(Page::MAIN_MENU); // Return to main menu
      return true;
    }
    if (event == ftxui::Event::Return) {
      if (bookCreationSuccessModal) {
        bookCreationSuccessModal = false;
        return true;
      }
      if (bookCreationFailureModal) {
        bookCreationFailureModal = false;
        return true;
      }
      if (orderAddSuccessModal) {
        orderAddSuccessModal = false;
        return true;
      }
      if (orderAddFailureModal) {
        orderAddFailureModal = false;
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
