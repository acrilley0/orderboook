#include "UserInterface.hpp"

ftxui::Component UserInterface::createMainMenu(const OrderBookManager& bookManager, int& currentPage, int &menuOptionSelected, std::vector<std::string>& symbols)
{
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
  return mainMenu;
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
