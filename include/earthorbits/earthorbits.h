#pragma once

#include <iostream>
#include <stdexcept>

namespace eob {
class EobError : public std::runtime_error {
 public:
  explicit EobError(const std::string &s) : std::runtime_error(s) {}
};

/// TODO how am I using this exception differently to warrant
/// it's existence?
class InvalidPrecondition : public std::runtime_error {
 public:
  explicit InvalidPrecondition(const std::string &s) : std::runtime_error(s) {}
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
Tle ParseTle(const std::string &str);
}  // namespace eob
