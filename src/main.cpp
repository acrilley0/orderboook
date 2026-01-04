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

ftxui::Component createSuccessModal(bool& successModalShown)
{
  return ftxui::Container::Vertical({
    ftxui::Renderer([&] {
      return ftxui::vbox({
        ftxui::text("Successfully created book!"),
      }) | STYLE;
    }),
    ftxui::Button("OK", [&] { successModalShown = false; }) | STYLE
  }) | STYLE;
}

ftxui::Component createFailureModal(bool& failureModalShown)
{
  return ftxui::Container::Vertical({
    ftxui::Renderer([&] {
      return ftxui::vbox({
        ftxui::text("Failed to create book!"),
      }) | ftxui::center | ftxui::border | ftxui::color(ftxui::Color::Red);
    }),
    ftxui::Button("OK", [&] { failureModalShown = false; }),
  });
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

  ftxui::MenuOption menuOption;
  menuOption.on_enter = [&] {
    switch (menuOptionSelected) {
      case static_cast<int>(Action::CREATE_BOOK): currentPage = static_cast<int>(Page::CREATE_BOOK_PAGE); break;
      case static_cast<int>(Action::LIST_BOOKS): currentPage = static_cast<int>(Page::LIST_BOOKS_PAGE); break;
    }
  };
  menuOption.Vertical();

  auto mainMenu = ftxui::Menu(&options, &menuOptionSelected, menuOption) | STYLE;

  // Page 1: Create Book
  std::string symbol;
  // OrderBook* bookp;
  bool inserted;
  ftxui::InputOption inputOption;
  bool successModalShown,failureModalShown = false;
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
  auto inputSymbol = ftxui::Input(&symbol, "Enter symbol: ", inputOption);
  auto createBookPage = ftxui::Container::Vertical({inputSymbol});

  // Page 2: List Books

  // Switch between components based on selectedTab
  auto tabContainer = ftxui::Container::Tab({
    mainMenu,
    createBookPage,
  }, &currentPage);

  auto mainWithBack = ftxui::Container::Vertical({
    ftxui::Renderer([] { return ftxui::text("Press [ESC] to return to the menu");} ),
    mainMenu,
  });

  auto createBookWithBack = ftxui::Renderer(createBookPage, [&] {
    return ftxui::vbox({
      inputSymbol->Render(),
    }) | STYLE;
  });

  auto allTabs = ftxui::Container::Tab({
    mainWithBack,
    createBookWithBack,
  }, &currentPage);

  auto finalContainer = ftxui::CatchEvent(allTabs, [&](ftxui::Event event) {
    if (event == ftxui::Event::Escape) {
      currentPage = 0; // Return to main menu
      return true;
    }
    return false;
  });

  finalContainer |=
    ftxui::Modal(createSuccessModal(successModalShown), &successModalShown);
  finalContainer |=
    ftxui::Modal(createFailureModal(failureModalShown), &failureModalShown);

  screen.Loop(finalContainer);

  return EXIT_SUCCESS;
}
