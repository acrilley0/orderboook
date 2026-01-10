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
  auto mainMenu = UserInterface::createMainMenu(bookManager, currentPage, menuOptionSelected, symbolList);

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

  // TESTING
  std::string symbolTest = "AAPL";
  bookManager.initBook(symbolTest);
  auto aaplbook = *bookManager.getBook(symbolTest);
  std::string price;
  std::string qty;
  bool orderAddSuccessModal = false;
  bool orderAddFailureModal = false;

  // Page 3: Add Order Page
  // auto addOrderPage = UserInterface::addOrderPage(aaplbook, symbolList, price, qty);
  auto addOrderPage = UserInterface::addOrderPage(aaplbook, symbolTest, price, qty, orderAddSuccessModal, orderAddFailureModal);
  tabs.push_back(addOrderPage);

  auto allTabs = ftxui::Container::Tab(tabs, &currentPage);
  auto withModals = allTabs;
  withModals |= ftxui::Modal(UserInterface::createResultModal(true, "Book was created!"), &bookCreationSuccessModal);
  withModals |= ftxui::Modal(UserInterface::createResultModal(false, "Failed to create book!"), &bookCreationFailureModal);
  withModals |= ftxui::Modal(UserInterface::createResultModal(false, "Order was added successfully!"), &orderAddSuccessModal);
  withModals |= ftxui::Modal(UserInterface::createResultModal(false, "Failed to add the order!"), &orderAddFailureModal);

  auto finalContainer = ftxui::CatchEvent(withModals, [&](ftxui::Event event) {
    if (event == ftxui::Event::Escape && !bookCreationSuccessModal && !bookCreationFailureModal) {
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
      return false;
    }
    return false;
  });

  screen.Loop(finalContainer);

  return EXIT_SUCCESS;
}
