#pragma once

#include "Utils.hpp"
#include <pqxx/pqxx>

class OrderBook;
class OrderBookManager;

class SecurityReference {
public:
  enum class AssetClass {
    NONE = 0,
    US_EQUITIES = 1,
  }; // TODO: This should have some kind of tostring func so that it can be represented in the Security Reference Data screen

  u32 security_id;
  std::string mic;
  std::string description;
  AssetClass asset_class;

  SecurityReference() :
    security_id(0),
    mic(""),
    description(""),
    asset_class(AssetClass::NONE)
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
