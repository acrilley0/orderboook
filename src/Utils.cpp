#include "Utils.hpp"

std::string trim(std::string_view str) {
  auto start = std::ranges::find_if_not(str.begin(), str.end(), ::isspace);
  if (start == str.end()) {
    return "";
  }

  auto end = std::ranges::find_if_not(str.rbegin(), str.rend(), ::isspace).base();

  return std::string(start, end);
}

bool isEmptyOrWhitespace(std::string_view str) {
  return trim(str).empty();
}

u64 getCurrentTime() {
  auto time = std::chrono::system_clock::now();
  auto dur = time.time_since_epoch();
  auto time_ms = dur.count();

  return time_ms;
}

bool tableExists(std::unique_ptr<pqxx::connection>& c, std::string_view table_name) {
  pqxx::nontransaction tx(*c.get()); // Read only transaction

  std::string query =
    "SELECT EXISTS ("
    " SELECT FROM information_schema.tables "
    " WHERE table_schema = 'public' "
    " AND table_name = " + tx.quote(table_name) +
    ")";

  pqxx::result res = tx.exec(query);

  return res[0][0].as<bool>();
}
