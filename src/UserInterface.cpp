#include "UserInterface.hpp"
#include "StringUtils.hpp"
#include <ftxui/component/component.hpp>
#include <ftxui/component/component_base.hpp>
#include <ftxui/component/component_options.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/dom/node.hpp>
#include <memory>

ftxui::Component UserInterface::createMainMenu(const OrderBookManager& bookManager, int& currentPage, int &menuOptionSelected, std::vector<std::string>& symbols)
{
  ftxui::MenuOption menuOption;
  menuOption.on_enter = [&] {
    switch (menuOptionSelected) {
      case static_cast<int>(Action::CREATE_BOOK): currentPage = static_cast<int>(Page::CREATE_BOOK_PAGE); break;
      case static_cast<int>(Action::LIST_BOOKS): {
        symbols = bookManager.getSymbols();
        if (symbols.size()) {
          currentPage = static_cast<int>(Page::LIST_BOOKS_PAGE);
        } else {
          // FIXME: Make an error popup saying there are currently no books
        }
        break;
      }
      case static_cast<int>(Action::ADD_ORDER): {
        currentPage = static_cast<int>(Page::ADD_ORDER_PAGE);
        break;
      }
    }
  };
  menuOption.Vertical();

  auto mainMenu = ftxui::Menu(&options, &menuOptionSelected, menuOption) | STYLE;

  return ftxui::Container::Vertical({
    ftxui::Renderer([] { return ftxui::text("Welcome to the OrderBook"); }) | ftxui::center | ftxui::color(ftxui::Color::Blue),
    mainMenu,
  });
}

ftxui::Component UserInterface::createBookPage(OrderBookManager& bookManager, std::string& symbol, bool& inserted, bool& successModalShown, bool& failureModalShown)
{
  ftxui::InputOption inputOption;
  inputOption.on_enter = [&] {
    std::string trimmed = trim(symbol);
    if (!trimmed.empty()) {
      inserted = bookManager.initBook(trimmed);
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
  auto labeledInput = ftxui::Container::Horizontal({
    ftxui::Renderer([] { return ftxui::text("Symbol: "); }),
    inputSymbol,
  });
  labeledInput->SetActiveChild(inputSymbol); // By default the active child would be the rendered text Element, so set it to the input manually
  labeledInput |= STYLE;

  return labeledInput;
}

ftxui::Component UserInterface::listBooksPage(std::vector<std::string>& symbols, int& selected)
{
  auto bookList = ftxui::Menu(&symbols, &selected, ftxui::MenuOption::Vertical());
  auto listWithTitle = ftxui::Container::Vertical({
    ftxui::Renderer([] { return ftxui::text("The following symbols currently have OrderBooks"); }),
    bookList,
  }) | STYLE;

  return listWithTitle;
}

ftxui::Component UserInterface::addOrderPage(OrderBook& book, std::string& priceStr, std::string& quantityStr)
{
  auto priceInput = ftxui::Input(&priceStr);
  auto quantityInput = ftxui::Input(&quantityStr);

  auto renderedOrderInfoInput = ftxui::Container::Vertical({
    ftxui::Container::Horizontal({
      ftxui::Renderer([] { return ftxui::text("Price:    "); }),
      priceInput,
    }),
    ftxui::Container::Horizontal({
      ftxui::Renderer([] { return ftxui::text("Quantity: "); }),
      quantityInput,
    }),
  });
  priceInput->TakeFocus();
  // How do I make the down arrow key go straight to quantityInput?
  renderedOrderInfoInput |= STYLE;

  // FIXME: The renderedPriceInput component needs an on_enter handler that calls
  // the addOrder function to actually add the order to the book

  return renderedOrderInfoInput;
}

ftxui::Component UserInterface::createResultModal(bool result, const std::string& message)
{
  ftxui::Element msg;
    msg = ftxui::vbox({ ftxui::text(message) }) |
      ftxui::center |
      ftxui::border |
      ftxui::size(ftxui::WIDTH, ftxui::LESS_THAN, 50);

  if (result) {
    msg |= ftxui::color(ftxui::Color::Green);
  } else {
    msg |= ftxui::color(ftxui::Color::Red);
  }

  auto msgToDisplay = ftxui::Container::Vertical({
    ftxui::Renderer([msg] {
      return msg;
    }),
  }) | ftxui::center;

  return msgToDisplay;
}
