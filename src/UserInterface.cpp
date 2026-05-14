#include "UserInterface.hpp"
#include "OrderBookManager.hpp"
#include "SecurityReference.hpp"
#include "Utils.hpp"
#include <ftxui/component/component.hpp>
#include <ftxui/component/component_base.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/dom/node.hpp>

inline std::string asset_class_to_string(SecurityReference::AssetClass asset_class) {
  switch (asset_class) {
    case SecurityReference::AssetClass::US_EQUITIES: {
      return "US Equities";
    }
    default:
      ERROR_LOG("Invalid asset class!");
      return "";
  }
}

ftxui::Component UserInterface::createMainMenu(int& current_page, int& menu_option_selected)
{
  ftxui::MenuOption menu_option;
  menu_option.on_enter = [&] {
    switch (menu_option_selected) {
      case static_cast<int>(Action::CREATE_BOOK): {
        current_page = static_cast<int>(Page::CREATE_BOOK_PAGE);
        break;
      }
      case static_cast<int>(Action::LIST_BOOKS): {
        current_page = static_cast<int>(Page::LIST_BOOKS_PAGE);
        break;
      }
      case static_cast<int>(Action::ADD_ORDER): {
        current_page = static_cast<int>(Page::ADD_ORDER_PAGE);
        break;
      }
      case static_cast<int>(Action::DISPLAY_REF_DATA): {
        current_page = static_cast<int>(Page::DISPLAY_REF_DATA);
        break;
      }
      case static_cast<int>(Action::DISPLAY_BOOK): {
        current_page = static_cast<int>(Page::DISPLAY_BOOK_PAGE);
        break;
      }
    }
  };
  menu_option.Vertical();

  auto mainMenu = ftxui::Menu(&options, &menu_option_selected, menu_option) | STYLE;

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

ftxui::Component UserInterface::createBookPage(OrderBookManager& book_manager,
                                std::string& symbol,
                                std::vector<std::string>& symbol_list,
                                modal_info_t& modal_info)
{
  ftxui::InputOption input_option;
  input_option.multiline = false;

  input_option.on_enter = [&book_manager, &symbol, &symbol_list, &modal_info] {
    std::string trimmed = trim(symbol);
    if (!trimmed.empty()) {
      modal_info.inserted = book_manager.initBook(trimmed);
      if (modal_info.inserted) {
        symbol_list.push_back(trimmed);
        modal_info.book_success_modal_shown = true;
      } else {
        modal_info.book_failure_modal_shown = true;
      }
      symbol.clear();
    }
  };
  auto input_symbol = ftxui::Input(&symbol, input_option) | ftxui::size(ftxui::WIDTH, ftxui::EQUAL, 30);
  auto labeled_input = ftxui::Container::Horizontal({
    ftxui::Renderer([] { return ftxui::text("Symbol: "); }),
    input_symbol,
  });
  labeled_input->SetActiveChild(input_symbol); // By default the active child would be the rendered text Element, so set it to the input manually
  labeled_input |= STYLE;

  return labeled_input;
}

ftxui::Component UserInterface::listBooksPage(std::vector<std::string>& symbols)
{
  auto symbolListContainer = ftxui::Container::Vertical({
    ftxui::Renderer([&symbols] {
      std::vector<ftxui::Element> symbol_list;
      for (const auto& symbol : symbols) {
        symbol_list.push_back(ftxui::text(symbol));
      }
      return ftxui::vbox(
        ftxui::text("The following symbols currently have OrderBooks:"),
        ftxui::separator(),
        symbol_list
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

ftxui::Component UserInterface::addOrderPage(OrderBookManager& book_manager,
                                             std::string& symbol,
                                             std::string& price_str,
                                             std::string& quantity_str,
                                             const std::vector<std::string>& sides,
                                             modal_info_t& modal_info)
{
  ftxui::InputOption inputOptions;
  inputOptions.multiline = false;

  inputOptions.on_enter = [&] {
    if (symbol.empty() || price_str.empty() || quantity_str.empty()) {
      modal_info.order_add_failure_modal_shown = true;
      return;
    }

    OrderBook* book = nullptr;
    try {
      book = book_manager.getBook(symbol);
    } catch (std::out_of_range&) {
      modal_info.order_add_failure_modal_shown = true;
      return;
    }

    Order newOrder = Order(getCurrentTime() + std::atoi(quantity_str.c_str()), // FIXME: Figure out a good way to generate order IDs
                           std::atoi(quantity_str.c_str()),
                           std::atof(price_str.c_str()),
                           getCurrentTime(),
                           static_cast<Side>(modal_info.selected_side),
                           symbol);

    OrderBook::order_execution_result_t result = book->addOrder(newOrder);
    if (result == OrderBook::ORDER_ADDED) {
      modal_info.order_add_success_modal_shown = true;
    } else if (result == OrderBook::ORDER_PARTIAL_FILL) {
      modal_info.order_partially_filled_modal_shown = true;
    } else if (result == OrderBook::ORDER_FILL) {
      modal_info.order_filled_modal_shown = true;
    }
  };

  auto symbolInput = ftxui::Input(&symbol, inputOptions);
  auto priceInput = ftxui::Input(&price_str, inputOptions);
  auto quantityInput = ftxui::Input(&quantity_str, inputOptions);

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

ftxui::Component UserInterface::printBookInfoModal(OrderBookManager& book_manager,
                                                   const std::vector<std::string>& symbol_list,
                                                   int& symbol_index)
{
  auto bookInfoRenderer = ftxui::Renderer([&book_manager, &symbol_list, &symbol_index] {
    std::string symbol;
    try {
      symbol = symbol_list.at(symbol_index);
    } catch (std::out_of_range&) {
      return ftxui::text("No book selected!") | ftxui::color(ftxui::Color::Red);
    }
    OrderBook* book = book_manager.getBook(symbol);
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
    }) | ftxui::size(ftxui::WIDTH, ftxui::EQUAL, 75) | ftxui::size(ftxui::HEIGHT, ftxui::EQUAL, 40) | ftxui::border;
  });

  return bookInfoRenderer;
}

ftxui::Component UserInterface::displayBookPage(std::vector<std::string>& symbol_list,
                                                int& selected_symbol,
                                                modal_info_t& modal_info)
{
  ftxui::MenuOption menu_options = {
    ftxui::MenuOption::Vertical(),
  };
  menu_options.on_enter = [&] {
    modal_info.current_symbol_for_modal = symbol_list[selected_symbol];
    modal_info.book_info_modal_shown = true;
  };

  auto list = ftxui::Menu(&symbol_list, &selected_symbol, menu_options);
  auto symbolSelectContainer = ftxui::Container::Vertical({
    ftxui::Renderer([&symbol_list] {
      if (symbol_list.empty()) {
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

ftxui::Component UserInterface::displaySecRefPage(std::vector<std::string>& symbol_list,
                                                  int& selected_symbol,
                                                  modal_info_t& modal_info) {
  // FIXME: This is pretty much identical to the displayBookPage...
  // I think we can show the same page in either case and then route
  // to print the proper modal based on which option was selected
  ftxui::MenuOption menu_options = {
    ftxui::MenuOption::Vertical(),
  };
  menu_options.on_enter = [&] {
    modal_info.current_symbol_for_modal = symbol_list[selected_symbol];
    modal_info.sec_ref_data_modal_shown = true;
  };

  auto list = ftxui::Menu(&symbol_list, &selected_symbol, menu_options);
  auto symbol_select_container = ftxui::Container::Vertical({
    ftxui::Renderer([&symbol_list] {
      if (symbol_list.empty()) {
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
  symbol_select_container->SetActiveChild(list);

  return symbol_select_container | STYLE;
}

ftxui::Component UserInterface::printSecRefModal(OrderBookManager &book_manager,
                                                 const std::vector<std::string> &symbol_list,
                                                 int &symbol_index) {
  auto sec_ref_data_renderer = ftxui::Renderer([&book_manager, &symbol_list, &symbol_index] {
    std::string symbol;
    try {
      symbol = symbol_list.at(symbol_index);
    } catch (std::out_of_range&) {
      return ftxui::text("No book selected!") | ftxui::color(ftxui::Color::Red);
    }
    OrderBook* book = book_manager.getBook(symbol);

    SecurityReference& sec_ref_data = book->sec_ref_data; // FIXME: This should be private and have accessors methods

    return ftxui::vbox({
        ftxui::text("Symbol: " + symbol),
        ftxui::separator(),
        ftxui::text("ID:          " + std::to_string(sec_ref_data.security_id)),
        ftxui::text("MIC:         " + sec_ref_data.mic),
        ftxui::text("Description: " + sec_ref_data.description),
        ftxui::text("Asset Class: " + asset_class_to_string(sec_ref_data.asset_class)),
    }) | STYLE;
  });

  return sec_ref_data_renderer;
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
