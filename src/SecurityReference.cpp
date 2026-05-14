#include "SecurityReference.hpp"
#include "OrderBookManager.hpp"
#include "Utils.hpp"
#include <string_view>

std::unique_ptr<pqxx::connection> SecurityReference::initConnection() { // FIXME: We should not coredump if we can't connect to the db for whatever reason
  try {
    const std::string conn_str = "user=postgres password=root host=localhost port=5432 dbname=security_reference_data";
    std::unique_ptr<pqxx::connection> c = std::make_unique<pqxx::connection>(conn_str);
    INFO_LOG("Successfully connected to security reference database");

    return c;
  } catch(pqxx::broken_connection& error) {
    ERROR_LOG("Failed to connect to security reference database. Security reference data will not be available.");
    ERROR_LOG(error.what());

    return nullptr;
  }
}

void SecurityReference::initTable(std::unique_ptr<pqxx::connection>& c) {
  bool symbols_table_exists = tableExists(c, "symbols");

  if (!symbols_table_exists) {
    pqxx::work w(*c.get());
    auto result = w.exec("CREATE TABLE symbols (name varchar(40), id int, MIC varchar(4), description varchar(100), AssetClass varchar(20))");
    w.commit();
  } else {
  }
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
      book->sec_ref_data.asset_class = (*it)["asset_class"];

      num_symbols_read++;
    }
  } catch (nlohmann::json_abi_v3_12_0::detail::parse_error& error) {
    ERROR_LOG("Security reference file not found!");
  }

  return num_symbols_read;
}
