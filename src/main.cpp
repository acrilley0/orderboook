#include "UserInterface.hpp"
#include <cstdlib>
#include <memory>

int main()
{
  OrderBookManager bookManager = OrderBookManager();

  auto screen = ftxui::ScreenInteractive::TerminalOutput();

  int currentPage = static_cast<int>(Page::MAIN_MENU);
  int menuOptionSelected = 0;
  std::vector<std::string> symbolList;
  auto mainMenu = UserInterface::createMainMenu(currentPage, menuOptionSelected);

  // Page 1: Create Book
  std::string symbol;
  bool inserted = false;
  bool bookCreationSuccessModal = false;
  bool bookCreationFailureModal = false;
  auto createBookPage = UserInterface::createBookPage(bookManager, symbol, symbolList, inserted, bookCreationSuccessModal, bookCreationFailureModal);

  std::vector<ftxui::Component> tabs = {mainMenu, createBookPage};

  // Page 2: List Books
  auto bookListPage = UserInterface::listBooksPage(symbolList);
  tabs.push_back(bookListPage);

  // Page 3: Add Order Page
  std::string price;
  std::string qty;
  int selectedSide = 0;
  bool orderAddSuccessModal = false;
  bool orderAddFailureModal = false;

  auto addOrderPage = UserInterface::addOrderPage(bookManager,
                                                  symbol,
                                                  price,
                                                  qty,
                                                  sides,
                                                  selectedSide,
                                                  orderAddSuccessModal,
                                                  orderAddFailureModal);
  tabs.push_back(addOrderPage);

  // Page 5: Display Book Page
  int selectedSymbol = 0;
  bool displayBookInfoModal = false;
  std::string currentSymbolForModal;
  auto displayBookPage = UserInterface::displayBookPage(symbolList,
                                                        selectedSymbol,
                                                        displayBookInfoModal,
                                                        currentSymbolForModal);
  tabs.push_back(displayBookPage);

  auto allTabs = ftxui::Container::Tab(tabs, &currentPage);
  auto withModals = allTabs;
  withModals |= ftxui::Modal(UserInterface::createResultModal(true, "Book was created!"), &bookCreationSuccessModal);
  withModals |= ftxui::Modal(UserInterface::createResultModal(false, "Failed to create book!"), &bookCreationFailureModal);
  withModals |= ftxui::Modal(UserInterface::createResultModal(true, "Order was added successfully!"), &orderAddSuccessModal);
  withModals |= ftxui::Modal(UserInterface::createResultModal(false, "Failed to add the order!"), &orderAddFailureModal);
  // withModals |= bookInfoModal;
  withModals |= ftxui::Modal(UserInterface::printBookInfoModal(bookManager, symbolList, selectedSymbol), &displayBookInfoModal);

  auto finalContainer = ftxui::CatchEvent(withModals, [&](ftxui::Event event) {
    if (event == ftxui::Event::Escape &&
        !bookCreationSuccessModal &&
        !bookCreationFailureModal &&
        !orderAddSuccessModal &&
        !orderAddFailureModal &&
        !displayBookInfoModal) {
      currentPage = static_cast<int>(Page::MAIN_MENU); // Return to main menu
      return true;
    }

    if (event == ftxui::Event::Escape && currentPage == static_cast<int>(Page::MAIN_MENU)) {
      // If the user hits Escape at the main menu, it should warn them that they are about to quit
    }

    if (event == ftxui::Event::Return) {
      if (bookCreationSuccessModal) {
        bookCreationSuccessModal = false;
        return true;
      }
      if (bookCreationFailureModal) {
        bookCreationFailureModal = false;
        return true;
      }
      if (orderAddSuccessModal) {
        orderAddSuccessModal = false;
        return true;
      }
      if (orderAddFailureModal) {
        orderAddFailureModal = false;
        return true;
      }
      if (displayBookInfoModal) {
        displayBookInfoModal = false;
        return true;
      }
      return false;
    }
    return false;
  });

  screen.Loop(finalContainer);

  return EXIT_SUCCESS;
}
