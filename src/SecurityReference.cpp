#include "SecurityReference.hpp"
#include "OrderBookManager.hpp"
#include <print>
#include <fstream>
#include <string_view>

std::ofstream out("psql_db_log.txt");

std::unique_ptr<pqxx::connection> SecurityReference::initConnection() { // FIXME: We should not coredump if we can't connect to the db for whatever reason
  const std::string conn_str = "user=postgres password=root host=localhost port=5432 dbname=security_reference_data";
  std::unique_ptr<pqxx::connection> c = std::make_unique<pqxx::connection>(conn_str);
  if (!c->is_open()) {
    std::println(out, "Error opening a connection to the database");
  }

  return c;
}

void SecurityReference::initTable(std::unique_ptr<pqxx::connection>& c) {
  std::println(out, "Connected to database \'{}\' successfully", c->dbname());
  bool symbols_table_exists = tableExists(c, "symbols");

  std::println(out, "Table exists? {}", symbols_table_exists);

  if (!symbols_table_exists) {
    pqxx::work w(*c.get());
    auto result = w.exec("CREATE TABLE symbols (name varchar(40), id int, MIC varchar(4), description varchar(100), AssetClass varchar(20))");
    std::println(out, "created table symbols");
    w.commit();
  } else {
    std::println(out, "symbols table has already been created!");
  }

  out.close();
}

u32 SecurityReference::readSymbolsJSON(const std::string& path,
                                       std::unique_ptr<pqxx::connection>& c,
                                       OrderBookManager& book_manager) {
  u32 num_symbols_read = 0;
  std::ifstream symbol_input_file(path);

  try {
    auto data = nlohmann::json::parse(symbol_input_file);
    for (auto it = data.begin(); it != data.end(); ++it) {
      book_manager.initBook((*it)["symbol"].get<std::string_view>());

      // Step 1: Create a database entry for each symbol
      pqxx::work w(*c);

      std::string query = "INSERT INTO symbols (name, id, mic, description, assetClass) VALUES (\'" +
        (*it)["symbol"].get<std::string>() + "\', \'" +
        nlohmann::to_string((*it)["id"]) + "\', \'" +
        (*it)["mic"].get<std::string>() + "\', \'" +
        (*it)["description"].get<std::string>() + "\', \'" +
        nlohmann::to_string((*it)["assetClass"]) + "\');";

      w.exec(query);
      w.commit();

      // Step 2: Fill out the SecurityReference object in the OrderBook for the symobl
      OrderBook* book = book_manager.getBook((*it)["symbol"]);
      book->sec_ref_data.security_id = (*it)["id"];
      book->sec_ref_data.mic = (*it)["mic"];
      book->sec_ref_data.description = (*it)["description"];
      book->sec_ref_data.asset_class = (*it)["assetClass"];

      num_symbols_read++;
    }
  } catch (nlohmann::json_abi_v3_12_0::detail::parse_error& error) {
    std::println(out, "Error parsing {} -- security reference data will not be available.", path);
    std::println(out, "{}", error.what());
  }

  out.close();

  return num_symbols_read;
}
