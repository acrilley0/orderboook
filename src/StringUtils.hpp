#include <string>
#include <algorithm>

inline std::string trim(const std::string& str)
{
  auto start = std::find_if_not(str.begin(), str.end(), ::isspace);
  if (start == str.end()) {
    return "";
  }

  auto end = std::find_if_not(str.rbegin(), str.rend(), ::isspace).base();

  return std::string(start, end);
}

inline bool isEmptyOrWhitespace(const std::string& str)
{
  return trim(str).empty();
}
