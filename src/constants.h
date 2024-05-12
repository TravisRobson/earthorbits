#pragma once

#include <numbers>

namespace eob {
/// @brief 2 * pi
constexpr double pi2 = 2.0 * std::numbers::pi;

constexpr double hours_per_day = 24;
constexpr double seconds_per_hour = 3600;
constexpr double seconds_per_day = seconds_per_hour * hours_per_day;
constexpr double ms_per_day = 1000 * seconds_per_day;
}  // namespace eob
