#include "UserInterface.hpp"
#include <ftxui/component/component.hpp>
#include <ftxui/component/component_base.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/dom/node.hpp>
#include <iostream> // FIXME: Only for testing
#include <stdexcept>

ftxui::Component UserInterface::createMainMenu(int& currentPage, int &menuOptionSelected)
{
  ftxui::MenuOption menuOption;
  menuOption.on_enter = [&] {
    switch (menuOptionSelected) {
      case static_cast<int>(Action::CREATE_BOOK): {
        currentPage = static_cast<int>(Page::CREATE_BOOK_PAGE);
        break;
      }
      case static_cast<int>(Action::LIST_BOOKS): {
        currentPage = static_cast<int>(Page::LIST_BOOKS_PAGE);
        break;
      }
      case static_cast<int>(Action::ADD_ORDER): {
        currentPage = static_cast<int>(Page::ADD_ORDER_PAGE);
        break;
      }
      case static_cast<int>(Action::DISPLAY_BOOK): {
        currentPage = static_cast<int>(Page::DISPLAY_BOOK_PAGE);
        break;
      }
    }
  };
  menuOption.Vertical();

  auto mainMenu = ftxui::Menu(&options, &menuOptionSelected, menuOption) | STYLE;

  return ftxui::Container::Vertical({
    ftxui::Renderer([] {
      return ftxui::vbox({
        ftxui::text("Welcome to the OrderBook"),
        ftxui::separator(),
      });
    }) | ftxui::center | ftxui::color(ftxui::Color::Blue),
    mainMenu,
  });
}

ftxui::Component UserInterface::createBookPage(OrderBookManager& bookManager,
                                               std::string& symbol,
                                               std::vector<std::string>& symbolList,
                                               bool& inserted,
                                               bool& successModalShown,
                                               bool& failureModalShown)
{
  ftxui::InputOption inputOption;
  inputOption.multiline = false;
  inputOption.on_enter = [&bookManager, &symbol, &symbolList, &inserted, &successModalShown, &failureModalShown] {
    std::string trimmed = trim(symbol);
    if (!trimmed.empty()) {
      inserted = bookManager.initBook(trimmed);
      if (inserted) {
        symbolList.push_back(trimmed);
        successModalShown = true;
      } else {
        failureModalShown = true;
      }
      symbol.clear();
    }
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
  auto list = ftxui::Menu(&symbols, &selected, ftxui::MenuOption::Vertical());
  auto symbolListContainer = ftxui::Container::Vertical({
    ftxui::Renderer([] { return ftxui::text("The following symbols currently have OrderBooks"); }) | ftxui::center,
    list,
  }) | STYLE;

  auto symbolListComponent = ftxui::Renderer([&symbols, symbolListContainer] {
    if (symbols.empty()) {
      return ftxui::text("There are currently no OrderBooks!") | ftxui::border | ftxui::color(ftxui::Color::Yellow) | ftxui::center;
    }
    return symbolListContainer->Render();
  });

  return symbolListComponent;
}

ftxui::Component UserInterface::addOrderPage(OrderBookManager& bookManager,
                                             std::string& symbol,
                                             std::string& priceStr,
                                             std::string& quantityStr,
                                             const std::vector<std::string>& sides,
                                             int& selectedSide,
                                             bool& successModalShown,
                                             bool& failureModalShown)
{
  ftxui::InputOption inputOptions;
  inputOptions.multiline = false;

  inputOptions.on_enter = [&] {
    if (symbol.empty() || priceStr.empty() || quantityStr.empty()) {
      failureModalShown = true;
      return;
    }

    OrderBook* book = bookManager.getBook(symbol);
    if (book == nullptr) {
      failureModalShown = true;
      return;
    }

    Order newOrder = Order(getCurrentTime(),
                           std::atoi(quantityStr.c_str()),
                           std::atof(priceStr.c_str()),
                           0,
                           static_cast<Side>(selectedSide),
                           symbol);

    bool orderAdded = book->addOrder(newOrder);
    if (orderAdded) {
      successModalShown = true;
    } else {
      failureModalShown = true;
    }
  };

  auto symbolInput = ftxui::Input(&symbol, inputOptions);
  auto priceInput = ftxui::Input(&priceStr, inputOptions);
  auto quantityInput = ftxui::Input(&quantityStr, inputOptions);

  auto symbolContainer = ftxui::Container::Horizontal({
    ftxui::Renderer([] { return ftxui::text("Symbol:   "); }),
    symbolInput,
  });
  symbolContainer->SetActiveChild(symbolInput);

  auto priceContainer = ftxui::Container::Horizontal({
    ftxui::Renderer([] { return ftxui::text("Price:    "); }),
    priceInput,
  });
  priceContainer->SetActiveChild(priceInput);

  auto quantityContainer = ftxui::Container::Horizontal({
    ftxui::Renderer([] { return ftxui::text("Quantity: "); }),
    quantityInput,
  });
  quantityContainer->SetActiveChild(quantityInput);

  auto sideContainer = ftxui::Radiobox(&sides, &selectedSide);

  auto inputContainer = ftxui::Container::Vertical({
    ftxui::Renderer([] {
      return ftxui::vbox({
        ftxui::text("Order Information"),
        ftxui::separator(),
      });
    }),
    symbolContainer,
    priceContainer,
    quantityContainer,
    ftxui::Renderer([] {
      return ftxui::vbox({
        ftxui::separator(),
        ftxui::text("Side:"),
      });
    }),
    sideContainer,
  });
  inputContainer->SetActiveChild(symbolContainer);
  inputContainer |= STYLE;

  return inputContainer;
}

ftxui::Component UserInterface::printBookInfoModal(const std::vector<std::string>& symbolList, int& index)
{
  std::string symbol;
  try {
    symbol = symbolList.at(index);
  } catch (std::out_of_range&) {
    return ftxui::Renderer([] { return ftxui::text("NO ORDERS"); });
  }

  // TODO: Call getBook(symbol) here and then print bids/asks
  auto bookInfo = ftxui::vbox({
    ftxui::text("Symbol: " + symbol) | ftxui::bold,
    ftxui::separator(),
    ftxui::text("BIDS") | ftxui::bold,
    ftxui::separator(),
    ftxui::text("ASKS") | ftxui::bold,
    ftxui::separator(),
  }) | ftxui::size(ftxui::WIDTH, ftxui::EQUAL, 50) | ftxui::size(ftxui::HEIGHT, ftxui::EQUAL, 50);

  auto bookInfoToDisplay = ftxui::Container::Vertical({
    ftxui::Renderer([bookInfo] {
      return bookInfo;
    }),
  }) | ftxui::center;

  return bookInfoToDisplay;
}

ftxui::Component UserInterface::displayBookPage(OrderBookManager& bookManager,
                                                std::vector<std::string>& symbolList,
                                                int& selectedSymbol,
                                                bool& displayBookInfoModal)
{
  ftxui::MenuOption menuOptions = {
    ftxui::MenuOption::Vertical(),
  };
  menuOptions.on_enter = [&bookManager, &symbolList, &selectedSymbol, &displayBookInfoModal] {
    OrderBook* book = bookManager.getBook(symbolList[selectedSymbol]);
    // We are guaranteed to have a book at this point
    displayBookInfoModal = true;
  };

  auto list = ftxui::Menu(&symbolList, &selectedSymbol, menuOptions);
  auto symbolSelectContainer = ftxui::Container::Vertical({
    ftxui::Renderer([&symbolList] {
      if (symbolList.empty()) {
        return ftxui::text("There are currently no OrderBooks!") | ftxui::border | ftxui::color(ftxui::Color::Yellow) | ftxui::center;
      } else {
        return ftxui::vbox({
          ftxui::text("Choose symbol to display Orders:"),
          ftxui::separator(),
        });
      }
    }),
    list | STYLE,
  });
  symbolSelectContainer->SetActiveChild(list);

  return symbolSelectContainer;
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
  }) | STYLE;

  return msgToDisplay;
}
