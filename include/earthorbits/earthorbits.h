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

/// @see https://celestrak.org/columns/v04n03/
struct TleLine1 {
  int line_number;
  int satellite_number;
  char classification;
  int launch_year;
  int launch_number;
  std::string launch_piece;
  int epoch_year;
  double epoch_day;
  double mean_motion_dot;   ///< time derivative of mean motion
  double mean_motion_ddot;  ///< double time derivative of mean motion
  double bstar_drag;
  int ephemeris_type;
  int element_number;
  int checksum;
};

/// @see https://celestrak.org/columns/v04n03/
struct TleLine2 {
  int line_number;
  int satellite_number;
  double inclination;           ///< degrees
  double raan;                  ///< degrees, Right Ascension of Ascending Node
  double eccentricity;          ///< [0, 1]
  double argument_of_perigree;  ///< degrees
  double mean_anomaly;          ///< degrees
  double mean_motion;           ///< revolutions per day
  int rev_at_epoch;             ///< revolution number at epoch
  int checksum;
};

/// @brief Two-line element
///
/// @see https://en.wikipedia.org/wiki/Two-line_element_set
/// @see https://celestrak.org/columns/v04n03/index.php#FAQ01
struct Tle {
  TleLine1 line_1;
  TleLine2 line_2;
};

std::ostream &operator<<(std::ostream &os, const Tle &tle);

/// @brief Convert string TLE to EOB struct
///
/// @throws EobError
/// Provides the Strong Exception Guarentee
///
/// @param str string containing all characters of one TLE
///
/// @pre str needs to have 69 * 2 + 1 = 139 characters
/// @pre str has linebreak at character 70
///
/// @return Tle struct containing parsed data
///
/// @post return valid (filled out) Tle instance
[[nodiscard]] Tle ParseTle(const std::string &str);
}  // namespace eob
