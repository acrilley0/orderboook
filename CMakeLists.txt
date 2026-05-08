cmake_minimum_required(VERSION 3.15)

project(OrderBook VERSION 1.0.0 LANGUAGES CXX)

enable_testing()
include(GoogleTest)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS_DEBUG "-g -Wall -Wextra -pedantic")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -Wall -Wextra -DNDEBUG")

# --- Fetch Dependencies ----------------------------------------------------------
include(FetchContent)
set(FETCHCONTENT_UPDATES_DISCONNECTED ON)
set(FETCHCONTENT_QUIET OFF)
FetchContent_Declare(
  ftxui
  GIT_REPOSITORY https://github.com/ArthurSonzogni/FTXUI
  GIT_TAG        v6.1.9
  GIT_PROGRESS   TRUE
  GIT_SHALLOW    TRUE
  EXCLUDE_FROM_ALL
)
FetchContent_MakeAvailable(ftxui)

FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
FetchContent_MakeAvailable(googletest)

# --- Main Executable ------------------------------------------------------------
set(SOURCES
    src/main.cpp
    src/OrderBook.cpp
    src/OrderBookManager.cpp
    src/UserInterface.cpp
    src/SecurityReference.cpp
    src/Utils.cpp
)
add_executable(orderbook ${SOURCES})

target_include_directories(orderbook PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
set_target_properties(orderbook PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../bin
)

FetchContent_Declare(
  libpqxx
  GIT_REPOSITORY https://github.com/jtv/libpqxx.git
  GIT_TAG        7.10.0
  GIT_SHALLOW    TRUE
  EXCLUDE_FROM_ALL
  SYSTEM
)
FetchContent_MakeAvailable(libpqxx)

FetchContent_Declare(
  json
  URL https://github.com/nlohmann/json/releases/download/v3.12.0/json.tar.xz
)
FetchContent_MakeAvailable(json)

target_link_libraries(orderbook
  PRIVATE ftxui::screen
  PRIVATE ftxui::dom
  PRIVATE ftxui::component
  PRIVATE libpqxx::pqxx
  PRIVATE nlohmann_json::nlohmann_json
)

# --- Test Configuration --------------------------------------------------------
set(TEST_SOURCES
  test/init_book.cpp
  test/add_order.cpp
)
set(BOOK_STRUCTURE_SOURCES
  src/OrderBook.cpp
  src/OrderBookManager.cpp
  src/Utils.cpp
)
add_executable(orderbook_tests ${BOOK_STRUCTURE_SOURCES} ${TEST_SOURCES})

target_include_directories(orderbook_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
set_target_properties(orderbook_tests PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../bin
)
target_link_libraries(orderbook_tests
  PRIVATE GTest::gtest_main
  PRIVATE libpqxx::pqxx
  PRIVATE nlohmann_json::nlohmann_json
)

gtest_discover_tests(orderbook_tests)
