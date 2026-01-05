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
  bool inserted;
  ftxui::InputOption inputOption;
  bool successModalShown = false;
  bool failureModalShown = false;
  inputOption.on_enter = [&] {
    if (!symbol.empty()) {
      inserted = bookManager.initBook(symbol);
      if (inserted) {
        successModalShown = true;
      } else {
        failureModalShown = true;
      }
      symbol.clear();
    }
    // FIXME: Figure out how not to create a book for an empty symbol
  };
  auto inputSymbol = ftxui::Input(&symbol, inputOption) | ftxui::size(ftxui::WIDTH, ftxui::EQUAL, 30);
  auto createBookPage = ftxui::Container::Vertical({inputSymbol});

  // Page 2: List Books
  int s = 0;
  auto bookListPage = ftxui::Menu(&symbols, &s, ftxui::MenuOption::Vertical());

  // Switch between components based on selectedTab
  auto tabContainer = ftxui::Container::Tab({
    mainMenu,
    createBookPage,
    bookListPage,
  }, &currentPage);

  auto mainWithBack = ftxui::Container::Vertical({
    ftxui::Renderer([] { return ftxui::text("Welcome to the OrderBook"); } ) | ftxui::center | ftxui::color(ftxui::Color::Blue),
    mainMenu,
  });

  auto createBookWithBack = ftxui::Renderer(createBookPage, [&] {
    return ftxui::hbox({
      ftxui::text("Symbol: "),
      inputSymbol->Render() | ftxui::size(ftxui::WIDTH, ftxui::LESS_THAN, 50),
    }) | STYLE;
  });

  auto listBookWithBack = ftxui::Container::Vertical({
      ftxui::Renderer([] { return ftxui::text("The following symbols currently have OrderBooks:"); }),
      bookListPage,
  }) | STYLE;

  auto allTabs = ftxui::Container::Tab({
    mainWithBack,
    createBookWithBack,
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
