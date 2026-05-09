#pragma once

#include "Utils.hpp"
#include <pqxx/pqxx>

class OrderBook;
class OrderBookManager;

class SecurityReference {
public:
  typedef enum {
    NONE = 0,
    US_EQUITIES = 1,
  } AssetClass;

  u32 security_id;
  std::string mic;
  std::string description;
  AssetClass asset_class;

  SecurityReference() :
    security_id(0),
    mic(""),
    description(""),
    asset_class(NONE)
  {}

  SecurityReference(u32 security_id, std::string mic, std::string description, AssetClass asset_class) :
    security_id(security_id),
    mic(mic),
    description(description),
    asset_class(asset_class)
  {}

  static std::unique_ptr<pqxx::connection> initConnection();
  static void initTable(std::unique_ptr<pqxx::connection>& c);
  static u32 readSymbolsJSON(const std::string& path, // Can't use std::string_view here because fstream requires a null terminated string (string_view is not null terminated)
                             std::unique_ptr<pqxx::connection>& c,
                             OrderBookManager& book_manager);
};
