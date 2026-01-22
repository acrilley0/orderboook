#include "Utils.hpp"

std::string trim(const std::string& str)
{
  auto start = std::ranges::find_if_not(str.begin(), str.end(), ::isspace);
  if (start == str.end()) {
    return "";
  }

  auto end = std::ranges::find_if_not(str.rbegin(), str.rend(), ::isspace).base();

  return std::string(start, end);
}

bool isEmptyOrWhitespace(const std::string& str)
{
  return trim(str).empty();
}

u32 getCurrentTime()
{
  auto time = std::chrono::system_clock::now();
  auto dur = time.time_since_epoch();
  auto time_ms = dur.count();

  return time_ms;
}
