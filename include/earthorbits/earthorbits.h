#pragma once

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

void foobar();
}  // namespace eob
