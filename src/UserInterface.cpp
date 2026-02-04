#include "UserInterface.hpp"
#include "Utils.hpp"
#include <ftxui/component/component.hpp>
#include <ftxui/component/component_base.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/dom/node.hpp>

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
                                modal_info_t& modal_info)
{
  ftxui::InputOption inputOption;
  inputOption.multiline = false;

  inputOption.on_enter = [&bookManager, &symbol, &symbolList, &modal_info] {
    std::string trimmed = trim(symbol);
    if (!trimmed.empty()) {
      modal_info.inserted = bookManager.initBook(trimmed);
      if (modal_info.inserted) {
        symbolList.push_back(trimmed);
        modal_info.book_success_modal_shown = true;
      } else {
        modal_info.book_failure_modal_shown = true;
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

ftxui::Component UserInterface::listBooksPage(std::vector<std::string>& symbols)
{
  auto symbolListContainer = ftxui::Container::Vertical({
    ftxui::Renderer([&symbols] {
      std::vector<ftxui::Element> symbolList;
      for (const auto& symbol : symbols) {
        symbolList.push_back(ftxui::text(symbol));
      }
      return ftxui::vbox(
        ftxui::text("The following symbols currently have OrderBooks:"),
        ftxui::separator(),
        symbolList
      ) | STYLE;
    }),
  });

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
                                             modal_info_t& modal_info)
{
  ftxui::InputOption inputOptions;
  inputOptions.multiline = false;

  inputOptions.on_enter = [&] {
    if (symbol.empty() || priceStr.empty() || quantityStr.empty()) {
      modal_info.order_add_failure_modal_shown = true;
      return;
    }

    OrderBook* book = nullptr;
    try {
      book = bookManager.getBook(symbol);
    } catch (std::out_of_range&) {
      modal_info.order_add_failure_modal_shown = true;
      return;
    }

    Order newOrder = Order(getCurrentTime() + std::atoi(quantityStr.c_str()), // FIXME: Figure out a good way to generate order IDs
                           std::atoi(quantityStr.c_str()),
                           std::atof(priceStr.c_str()),
                           getCurrentTime(),
                           static_cast<Side>(modal_info.selected_side),
                           symbol);

    OrderBook::order_execution_result_t result = book->addOrder(newOrder);
    if (result == OrderBook::ORDER_ADDED) {
      modal_info.order_add_success_modal_shown = true;
    } else if (result == OrderBook::ORDER_PARTIAL_FILL){
      modal_info.order_partially_filled_modal_shown = true;
    } else if (result == OrderBook::ORDER_FILL) {
      modal_info.order_filled_modal_shown = true;
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

  auto sideContainer = ftxui::Radiobox(&sides, &modal_info.selected_side);

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

ftxui::Component UserInterface::printBookInfoModal(OrderBookManager& bookManager,
                                                   const std::vector<std::string>& symbolList,
                                                   int& symbolIndex)
{
  auto bookInfoRenderer = ftxui::Renderer([&bookManager, &symbolList, &symbolIndex] {
    std::string symbol;
    try {
      symbol = symbolList.at(symbolIndex);
    } catch (std::out_of_range&) {
      return ftxui::text("No book selected!") | ftxui::color(ftxui::Color::Red);
    }
    OrderBook* book = bookManager.getBook(symbol);
    if (book == nullptr) {
      return ftxui::text("No book selected!") | ftxui::color(ftxui::Color::Red);
    }

    int levelCounter = 1;
    std::vector<ftxui::Element> bidLevels;
    for (const auto & priceLevel : book->bids) {
      for (const auto & order : priceLevel.second) {
        bidLevels.push_back(ftxui::text("Level[" + std::to_string(levelCounter) + "] -- " +
                                        "Price: " + std::to_string(order.price) +
                                        " Quantity: " + std::to_string(order.quantity) +
                                        " Timestamp: " + std::to_string(order.timestamp)));
      }
      bidLevels.push_back(ftxui::separator());
      levelCounter++;
    }

    levelCounter = 1;
    std::vector<ftxui::Element> askLevels;
    for (const auto & priceLevel : book->asks) {
      for (const auto & order : priceLevel.second) {
        askLevels.push_back(ftxui::text("Level[" + std::to_string(levelCounter) + "] -- " +
                                        "Price: " + std::to_string(order.price) +
                                        " Quantity: " + std::to_string(order.quantity) +
                                        " Timestamp: " + std::to_string(order.timestamp)));
      }
      askLevels.push_back(ftxui::separator());
      levelCounter++;
    }

    return ftxui::vbox({
      ftxui::text("Symbol: " + book->symbol) | ftxui::color(ftxui::Color::Green3) | ftxui::bold | ftxui::center,
      ftxui::separator(),
      ftxui::text("BIDS") | ftxui::color(ftxui::Color::Green3) | ftxui::bold,
      ftxui::separator(),
      ftxui::vbox(bidLevels) | ftxui::size(ftxui::WIDTH, ftxui::EQUAL, 80) | ftxui::size(ftxui::HEIGHT, ftxui::EQUAL, 80),
      ftxui::separator(),
      ftxui::text("ASKS") | ftxui::color(ftxui::Color::Green3)| ftxui::bold,
      ftxui::separator(),
      ftxui::vbox(askLevels) | ftxui::size(ftxui::WIDTH, ftxui::EQUAL, 80) | ftxui::size(ftxui::HEIGHT, ftxui::EQUAL, 80),
    }) | ftxui::size(ftxui::WIDTH, ftxui::EQUAL, 75) | ftxui::border;
  });

  return bookInfoRenderer;
}

ftxui::Component UserInterface::displayBookPage(std::vector<std::string>& symbolList,
                                                int& selectedSymbol,
                                                modal_info_t& modal_info)
{
  ftxui::MenuOption menuOptions = {
    ftxui::MenuOption::Vertical(),
  };
  menuOptions.on_enter = [&] {
    modal_info.current_symbol_for_modal = symbolList[selectedSymbol];
    modal_info.book_info_modal_shown = true;
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
    list,
  });
  symbolSelectContainer->SetActiveChild(list);

  return symbolSelectContainer | STYLE;
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
