#pragma once

#include <stdint.h>
#include <string>
#include <vector>
#include <chrono>

using i32  = int32_t;
using i64  = int64_t;
using u8   = uint8_t;
using u32  = uint32_t;
using u64  = uint64_t;

const std::vector<std::string> sides = {"BID", "ASK"};

std::string trim(const std::string& str);
bool isEmptyOrWhitespace(const std::string& str);
u32 getCurrentTime();
