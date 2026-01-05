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
  std::vector<std::string> symbols;
  auto mainMenu = UserInterface::createMainMenu(bookManager, currentPage, menuOptionSelected, symbols);

  // Page 1: Create Book
  std::string symbol;
  bool inserted = false;
  bool successModalShown = false;
  bool failureModalShown = false;
  auto createBookPage = UserInterface::createBookPage(bookManager, symbol, inserted, successModalShown, failureModalShown);

  // Page 2: List Books
  int selected = 0;
  auto bookListPage = UserInterface::listBooksPage(symbols, selected);

  // FIXME: Get rid of this...
  // Temp to guarantee a book exists on the add order page
  std::string price;
  std::string quantity;
  bookManager.initBook("AAPL");
  auto aaplBook = bookManager.getBook("AAPL");
  auto addOrderPage = UserInterface::addOrderPage(*aaplBook, price, quantity);

  auto allTabs = ftxui::Container::Tab({
    mainMenu,
    createBookPage,
    bookListPage,
    addOrderPage,
  }, &currentPage);
  auto withModals = allTabs;
  withModals |= ftxui::Modal(UserInterface::createResultModal(true, "Book was created!"), &successModalShown);
  withModals |= ftxui::Modal(UserInterface::createResultModal(false, "Failed to create book!"), &failureModalShown);

  auto finalContainer = ftxui::CatchEvent(withModals, [&](ftxui::Event event) {
    if (event == ftxui::Event::Escape && !successModalShown && !failureModalShown) {
      currentPage = static_cast<int>(Page::MAIN_MENU); // Return to main menu
      return true;
    }
    if (event == ftxui::Event::Return) {
      if (successModalShown) {
        successModalShown = false;
        return true;
      }
      if (failureModalShown) {
        failureModalShown = false;
        return true;
      }
      return false;
    }
    return false;
  });

  screen.Loop(finalContainer);

  return EXIT_SUCCESS;
}
