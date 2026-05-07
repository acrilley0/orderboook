#pragma once

#include <stdint.h>
#include <string>
#include <vector>
#include <pqxx/pqxx>

using i32  = int32_t;
using i64  = int64_t;
using u8   = uint8_t;
using u32  = uint32_t;
using u64  = uint64_t;

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

std::string trim(const std::string& str);
bool isEmptyOrWhitespace(const std::string& str);
u64 getCurrentTime();
bool tableExists(pqxx::connection &c, const std::string& table_name);
