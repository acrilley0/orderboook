#pragma once

#include <string>
#include <vector>
#include <pqxx/pqxx>
#include <nlohmann/json.hpp>


#ifndef _DEBUG
  #include <fstream>
  inline std::ofstream log_file("debug.log");

  #define INFO_LOG(log_statement) std::println(log_file, "\033[32m[INFO]\033[0m -- {}", log_statement);
  #define WARNING_LOG(x) std::println(log_file, "\033[33m[WARNING]\033[0m -- {}", x);
  #define ERROR_LOG(x) std::println(log_file, "\033[31m[ERROR]\033[0m -- {}", x);
#else
  #define INFO_LOG(x)
  #define WARNING_LOG(x)
  #define ERROR_LOG(x)
#endif

using i32  = int32_t;
using i64  = int64_t;
using u8   = uint8_t;
using u32  = uint32_t;
using u64  = uint64_t;
using f64  = double;

struct modal_info_t {
  bool inserted; // Indicates whether an order was inserted in the globalOrderIndex
  int selected_side; // Indicates which side an order is for when on the addOrderPage
  // The following flags indicate if success/failure modals should be shown
  // for the book creation and add order pages
  bool book_success_modal_shown;
  bool book_failure_modal_shown;
  bool order_add_success_modal_shown;
  bool order_add_failure_modal_shown;
  bool order_filled_modal_shown;
  bool order_partially_filled_modal_shown;
  bool book_info_modal_shown;
  std::string current_symbol_for_modal; // Tracks which symbol book info (bids and asks) should be displayed for

  modal_info_t() {
    inserted = false;
    selected_side = 0;
    book_success_modal_shown = false;
    book_failure_modal_shown = false;
    order_add_success_modal_shown = false;
    order_add_failure_modal_shown = false;
    order_filled_modal_shown = false;
    order_partially_filled_modal_shown = false;
    book_info_modal_shown = false;
    current_symbol_for_modal = "";
  }
};

typedef enum {
  SUCCESS,
  FAILURE,
} book_modification_result_t;

const std::vector<std::string> sides = {"BID", "ASK"};

std::string trim(std::string_view str);
bool isEmptyOrWhitespace(std::string_view str);
u64 getCurrentTime();
bool tableExists(std::unique_ptr<pqxx::connection>& c, std::string_view table_name);
