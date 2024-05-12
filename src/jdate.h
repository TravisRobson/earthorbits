#pragma once

#include <chrono>

#include "date/date.h"

/// @see https://stackoverflow.com/a/33964462
/// TODO delete this file depending on whether I need it actually or not

constexpr auto jdiff() {
  using namespace date;
  using namespace std::chrono_literals;
  return sys_days{January / 1 / 1970} - (sys_days{November / 24 / -4713} + 12h);
}

/// @brief Clock representing Julian days
///
/// std::ratio<86400> means the tick is in units of days, but
/// fractional (rep = double), just like Julian days are typically
/// represented
struct jdate_clock {
  // https://en.cppreference.com/w/cpp/named_req/Clock
  // ^ clock requirements for std::chrono::time_point<Clock, duration>
  using rep = double;
  using period = std::ratio<86400>;
  using duration = std::chrono::duration<rep, period>;
  using time_point = std::chrono::time_point<jdate_clock>;

  static constexpr bool is_steady = false;

  static time_point now() noexcept {
    using namespace std::chrono;
    return time_point{duration{system_clock::now().time_since_epoch()} +
                      jdiff()};
  }
};

template <class Duration>
constexpr auto sys_to_jdate(
    std::chrono::time_point<std::chrono::system_clock, Duration> tp) noexcept {
  using namespace std::chrono;
  static_assert(jdate_clock::duration{jdiff()} < Duration::max(),
                "Overflow in sys_to_jdate");
  const auto d = tp.time_since_epoch() + jdiff();
  return time_point<jdate_clock, std::remove_cv_t<decltype(d)>>{d};
}

template <class Duration>
constexpr auto jdate_to_sys(
    std::chrono::time_point<jdate_clock, Duration> tp) noexcept {
  using namespace std::chrono;
  static_assert(jdate_clock::duration{-jdiff()} > Duration::min(),
                "Overflow in jdate_to_sys");
  const auto d = tp.time_since_epoch() - jdiff();
  return time_point<system_clock, std::remove_cv_t<decltype(d)>>{d};
}
