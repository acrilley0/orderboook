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
  auto bookPage = UserInterface::createBookPage(bookManager, symbol, inserted, successModalShown, failureModalShown);

  // Page 2: List Books
  int s = 0;
  auto bookListPage = ftxui::Menu(&symbols, &s, ftxui::MenuOption::Vertical());

  auto listBookWithBack = ftxui::Container::Vertical({
      ftxui::Renderer([] { return ftxui::text("The following symbols currently have OrderBooks:"); }),
      bookListPage,
  }) | STYLE;

  auto allTabs = ftxui::Container::Tab({
    mainMenu,
    bookPage,
    listBookWithBack,
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
