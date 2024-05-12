#pragma once

#include <chrono>
#include <iostream>
#include <source_location>
#include <stdexcept>

namespace eob {
inline std::ostream &operator<<(std::ostream &os,
                                const std::source_location &loc) {
  return os << loc.file_name() << ':' << loc.line() << ", function `"
            << loc.function_name() << '`';
}

template <typename DATA_T>
class MyException {
 public:
  MyException(std::string str, DATA_T data,
              const std::source_location &loc = std::source_location::current())
      : errorStr_{std::move(str)}, data_{std::move(data)}, location_{loc} {}

  std::string &what() { return errorStr_; }
  const std::string &what() const noexcept { return errorStr_; }
  DATA_T &data() { return data_; }
  const DATA_T &data() const noexcept { return data_; }
  const std::source_location &where() const noexcept { return location_; }

 private:
  std::string errorStr_;
  DATA_T data_;
  std::source_location location_;
};

[[nodiscard]] std::string to_string(
    const std::chrono::time_point<std::chrono::system_clock> &tp);

/// @brief Compute sidereal time, greenwich mean sidereal time
/// @see https://celestrak.org/columns/v02n02/
/// @return seconds, wrapped to 86400 seconds
[[nodiscard]] double calc_gmst(
    const std::chrono::time_point<std::chrono::system_clock> &tp) noexcept;
}  // namespace eob
