#include "OrderBookManager.hpp"
#include <cstdlib>
#include <ftxui/component/component.hpp>
#include <ftxui/component/component_options.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/color.hpp>
#include <ftxui/util/ref.hpp>
#include <memory>
#include <chrono>
#include <iostream>

#define STYLE ftxui::border | ftxui::color(ftxui::Color::Blue) | ftxui::size(ftxui::WIDTH, ftxui::LESS_THAN, 50) | ftxui::center

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

ftxui::Component createResultModal(bool& shown, bool result, const std::string& message) {
  ftxui::Element displayedMsg;
  if (result)
    displayedMsg = ftxui::vbox({ ftxui::text(message) }) |
      ftxui::center |
      ftxui::border |
      ftxui::color(ftxui::Color::Green) |
      ftxui::size(ftxui::WIDTH, ftxui::LESS_THAN, 50);
  else
    displayedMsg = ftxui::vbox({ ftxui::text(message) }) |
      ftxui::center |
      ftxui::border |
      ftxui::color(ftxui::Color::Red) |
      ftxui::size(ftxui::WIDTH, ftxui::LESS_THAN, 50);

  return ftxui::Container::Vertical({
    ftxui::Renderer([displayedMsg] {
      return displayedMsg;
    }),
    ftxui::Button("OK", [&shown] { shown = false; }) | STYLE
  }) | ftxui::center;
}

int main()
{
  OrderBookManager bookManager = OrderBookManager();

  auto screen = ftxui::ScreenInteractive::TerminalOutput();

  const std::vector<std::string> options = {
    "1. Create an OrderBook",
    "2. List symbols that currently have OrderBooks",
    "3. Add an Order to an OrderBook",
    "4. Modify existing order",
    "5. Display an order book",
  };

  // Page 0: Main Menu
  int currentPage = 0;
  int menuOptionSelected = 0;

  std::vector<std::string> symbols;
  ftxui::MenuOption menuOption;
  menuOption.on_enter = [&] {
    switch (menuOptionSelected) {
      case static_cast<int>(Action::CREATE_BOOK): currentPage = static_cast<int>(Page::CREATE_BOOK_PAGE); break;
      case static_cast<int>(Action::LIST_BOOKS): {
        symbols = bookManager.getSymbols();
        if (symbols.size())
          currentPage = static_cast<int>(Page::LIST_BOOKS_PAGE);
        else
          // FIXME: Make an error popup saying there are currently no books
        break;
      }
    }
  };
  menuOption.Vertical();

  auto mainMenu = ftxui::Menu(&options, &menuOptionSelected, menuOption) | STYLE;

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

  auto finalContainer = ftxui::CatchEvent(allTabs, [&](ftxui::Event event) {
    if (event == ftxui::Event::Escape) {
      currentPage = 0; // Return to main menu
      return true;
    }
    return false;
  });

  finalContainer |=
    ftxui::Modal(createResultModal(successModalShown, true, "Book was created!"), &successModalShown);
  finalContainer |=
    ftxui::Modal(createResultModal(failureModalShown, false, "Failure to create book!"), &failureModalShown);

  screen.Loop(finalContainer);

  return EXIT_SUCCESS;
}
