#include "earthorbits/parsetle.h"

#include <fmt/core.h>
#include <fmt/ostream.h>

#include <algorithm>
#include <array>
#include <cassert>
#include <cctype>
#include <cstddef>
#include <functional>
#include <limits>
#include <optional>
#include <sstream>
#include <string>
#include <string_view>

#include "earthorbits/earthorbits.h"

template <>
struct fmt::formatter<eob::Tle> : ostream_formatter {};

namespace eob {
namespace {
constexpr int tle_line_size = 69;

static_assert(sizeof(char) == 1, "Have only programmed assuming char is 1B");
static_assert(
    sizeof(size_t) >= sizeof(int),
    "platform doesn't support correction function of safe_int_to_size_t");

/// @brief Safely convert int to size_t
/// @see https://stackoverflow.com/a/27513865
[[nodiscard]] constexpr size_t safe_int_to_size_t(int val) noexcept {
  return (val < 0) ? __SIZE_MAX__
                   : static_cast<size_t>(static_cast<unsigned>(val));
}

constexpr std::string_view tle_valid_chars =
    "ABCDEFGHIJKLMNOPQRSTUV+- 0123456789.\n";
constexpr auto max_char_as_int =
    safe_int_to_size_t(std::numeric_limits<char>::max());

[[nodiscard]] constexpr std::array<bool, max_char_as_int>
get_valid_tle_char_mask() {
  std::array<bool, max_char_as_int> mask{};  // initialize all to false
  // mark valid characters as true
  for (char c : tle_valid_chars) {
    mask.at(safe_int_to_size_t(c)) = true;
  }
  return mask;
}

/// @brief Check if all characters of string exist in valid character list
/// @param str string to be checked
/// @param valid_chars string containing list of valid characters
///
/// @see https://codereview.stackexchange.com/a/39957
///
/// @return if valid return std::nullopt, else return position of first invalid
/// char
[[nodiscard]] bool contains_valid_tle_chars(const std::string &str) noexcept {
  auto mask = get_valid_tle_char_mask();
  return !std::any_of(str.begin(), str.end(), [&mask](char c) {
    // didn't use std::array::at because how mask is constructed it should
    // be impossible to exceed its bounds
    return !mask[safe_int_to_size_t(c)];
  });
}

/// @brief Convert TLE "exponential" string to double
/// @param sub_str TLE line substring containing the exponential
///
/// @pre Expects string to have at least 3 characters
///
/// @throws MyException if sub_str has unexpected format
/// @throw std::out_of_range from std::stod
/// @throws std::invalid_argument from std::stod
///
/// @return double, coverted from sub_str
[[nodiscard]] double exponent_to_double(const std::string &sub_str) {
  assert(sub_str.size() > 3 && "exponent_to_double() string not large enough");

  double prefix_sign = 1.0;
  switch (sub_str[0]) {
    case '-':
      prefix_sign = -1.0;
      break;
    case '+':
    case ' ':
      prefix_sign = 1.0;
      break;
    default:
      throw MyException<std::string>(
          fmt::format(
              R"(TLE contains invalid exponential field, expected "+", "-", or " ", found="{}")",
              sub_str[0]),
          sub_str);
  }

  double exp_sign = 1.0;
  switch (sub_str[sub_str.size() - 2]) {
    case '-':
      exp_sign = -1.0;
      break;
    case '+':
      exp_sign = 1.0;
      break;
    default:
      throw MyException<std::string>(
          fmt::format(
              R"(TLE contains invalid exponential field, expected "+" or "-", found="{}")",
              sub_str[sub_str.size() - 2]),
          sub_str);
  }

  // skip first character when finding position of sign
  double left = std::stod("0." + sub_str.substr(1, sub_str.size() - 2));
  double right = std::stod(sub_str.substr(sub_str.size() - 1));

  return prefix_sign * left * std::pow(10.0, exp_sign * right);
}

/// The checksum is (Modulo 10) (Letters, blanks, periods, plus signs = 0; minus
/// signs = 1)
[[nodiscard]] int compute_checksum(const std::string &line) {
  assert(!line.empty() && "line should have at least one character");
  int sum = 0;
  for (char c : line.substr(0, line.size() - 1)) {
    if (std::isdigit(c) != 0) {
      sum += static_cast<int>(c - '0');
    } else if (c == '-') {
      sum += 1;
    }
  }

  return sum % 10;
}

/// @brief Check domain of parameter inclusive [lower_bound, upper_bound]
[[nodiscard]] std::optional<std::string> is_within_inclusive_domain(
    double value, double lower_bound, double upper_bound) {
  assert(lower_bound < upper_bound && "lower_bound must be < upper_bound");
  if (lower_bound <= value && value <= upper_bound) {
    return std::nullopt;
  }

  // unfortunately this lint would lead to less performant code
  /// NOLINTNEXTLINE(modernize-return-braced-init-list])
  return std::optional<std::string>(
      fmt::format(R"(value={}, lower_bound={}, upper_bound={})", value,
                  lower_bound, upper_bound));
}
}  // namespace

/// TODO(tjr) need to determine the exceptions this function can
/// throw, but at least right now we only use for tests
std::ostream &operator<<(std::ostream &os, const Tle &tle) {
  os << fmt::format(
      R"({{line_number={}, satellite_number={}, classification={}, launch_year={}, )"
      R"(launch_number={}, launch_piece="{}", epoch_year={}, epoch_day={}, mean_motion_dot={}, )"
      R"(mean_motion_ddot={}, bstar_drag={}, ephemeris_type={}, element_number={}, )"
      R"(checksum={}}})",
      tle.line_1.line_number, tle.line_1.satellite_number,
      tle.line_1.classification, tle.line_1.launch_year,
      tle.line_1.launch_number, tle.line_1.launch_piece, tle.line_1.epoch_year,
      tle.line_1.epoch_day, tle.line_1.mean_motion_dot,
      tle.line_1.mean_motion_ddot, tle.line_1.bstar_drag,
      tle.line_1.ephemeris_type, tle.line_1.element_number,
      tle.line_1.checksum);
  os << ", ";
  return os << fmt::format(
             R"({{line_number={}, satellite_number={}, inclination={}°, )"
             R"(raan={}°, eccentricity={}, argument_of_perigree={}°, )"
             R"(mean_anomaly={}°, mean_motion={}, rev_at_epoch={}, checksum={}))}})",
             tle.line_2.line_number, tle.line_2.satellite_number,
             tle.line_2.inclination, tle.line_2.raan, tle.line_2.eccentricity,
             tle.line_2.argument_of_perigree, tle.line_2.mean_anomaly,
             tle.line_2.mean_motion, tle.line_2.rev_at_epoch,
             tle.line_2.checksum);
}

/// Example:
///   1 25544U 98067A   24097.81509284  .00011771  00000-0  21418-3 0  9995
///   2 25544  51.6405 309.2692 0004792  43.0163  63.5300 15.49960977447473
///
///   But now let's label the pieces
///
///   123456789-123456789-123456789-123456789-123456789-123456789-123456789
///   A BBBBBC DDEEEFFF GGHHHHHHHHHHHH IIIIIIIIII JJJJJJJJ KKKKKKKK L MMMMN
///   1 25544U 98067A   24097.81509284  .00011771  00000-0  21418-3 0  9995
///   2 25544  51.6405 309.2692 0004792  43.0163  63.5300 15.49960977447473
///   O PPPPP QQQQQQQQ RRRRRRRR SSSSSSS TTTTTTTT UUUUUUUU VVVVVVVVVVVWWWWWX
///
///   Line 1
///     [1] A = TLE line number {1}
///     [3, 7] BBBBB = satellite number {25544}
///     [8] C = classification {U}
///     [10, 11] DD = International Designator
///                (Last two digits of launch year) {98}
///     [12, 14] EEE = International Designator
///                (Launch number of the year) {067}
///     [15, 17] FFF = International Designator
///                (Piece of the launch) {A }
///     [19, 20] GG = Epoch Year (Last two digits of year) {24}
///     [21, 32] HHHHHHHHHHHH = Epoch (Day of the year
///                and fractional portion ofthe day) {097.81509284}
///     [34, 43] IIIIIIIIII = First Time Derivative of
///                the Mean Motion { .00011771}
///     [45, 52] JJJJJJJJ = Second Time Derivative of Mean Motion
///                (decimal point assumed) {JJJJJJJJ}
///     [54, 61] KKKKKKKK = BSTAR drag term (decimal point
///                assumed) { 21418-3}
///     [63] L = Ephemeris type {0}
///     [65, 68] MMMMM = Element number { 999}
///     [69] N = Checksum {9}
///
///   Line 2
///     [1] O = TLE line number
///     [3, 7] PPPP = satellite number {25544}
///     [9, 16] QQQQQQQQ = inclination (degrees) { 51.6405}
///     [18, 25] RRRRRRRR = Right Ascension of the
///                Ascending Node (degrees) {309.2692}
///     [27, 33] SSSSSSS = Eccentricity {0004792}
///     [35, 42] TTTTTTTT = Argument of Perigee
///                (degrees) { 43.0163}
///     [44, 51] UUUUUUUU = Mean Anomaly (degrees) { 63.5300}
///     [53, 63] VVVVVVVVVVV = Mean Motion {15.49960977}
///     [64, 68] WWWWW = Revolution number at epoch {44747}
///     [69] X = Checksum {3}
///
/// The checksum is (Modulo 10) (Letters, blanks, periods, plus signs = 0; minus
/// signs = 1)
///
/// TODO(tjr) can any of error logic code be consolidated?
/// NOLINTNEXTLINE(readability-function-cognitive-complexity)
[[nodiscard]] Tle ParseTle(const std::string &tle_str) {
  // two lines of 69 characters and a line break
  constexpr int expected_length = 2 * tle_line_size + 1;
  if (tle_str.size() != expected_length) {
    throw MyException<std::string>(
        fmt::format(R"(TLE has invalid size, size={}, expected={})",
                    tle_str.size(), expected_length),
        tle_str);
  }

  if (tle_str[tle_line_size] != '\n') {
    throw MyException<std::string>(
        fmt::format(
            R"(TLE invalid, expected line break at position={:d}, found="{}")",
            tle_line_size, tle_str[tle_line_size]),
        tle_str);
  }

  if (!contains_valid_tle_chars(tle_str)) {
    throw MyException<std::string>(
        fmt::format(R"(TLE contains invalid char(s))"), tle_str);
  }

  Tle tle;
  std::stringstream ss(tle_str);

  std::string line_1;
  std::getline(ss, line_1, '\n');
  assert(line_1.size() == tle_line_size &&
         "TLE line 1 should be 69 characters");
  if (ss.fail()) {
    throw MyException<std::string>(fmt::format(R"(failed to read TLE line 1)"),
                                   tle_str);
  }

  std::string line_2;
  std::getline(ss, line_2, '\n');
  assert(line_2.size() == tle_line_size &&
         "TLE line 2 should be 69 characters");
  if (ss.fail()) {
    throw MyException<std::string>(fmt::format(R"(failed to read TLE line 2)"),
                                   tle_str);
  }
  assert(ss.eof() && "Should only be two lines in TLE stringstream");

  struct PosSizeParser {
    size_t start;
    size_t size;
    std::function<void(const std::string &)> parser;
  };

  /// TODO is there a way to accomplish this without std::function?
  std::array<PosSizeParser, 14> line_1_parsers{
      PosSizeParser{
          .start = 0,
          .size = 1,
          .parser =
              [&tle](const std::string &substr) {
                tle.line_1.line_number = std::stoi(substr);
              },
      },
      PosSizeParser{
          .start = 2,
          .size = 5,
          .parser =
              [&tle](const std::string &substr) {
                tle.line_1.satellite_number = std::stoi(substr);
              },
      },
      PosSizeParser{
          .start = 7,
          .size = 1,
          .parser =
              [&tle](const std::string &substr) {
                tle.line_1.classification = substr[0];
              },
      },
      PosSizeParser{
          .start = 9,
          .size = 2,
          .parser =
              [&tle](const std::string &substr) {
                tle.line_1.launch_year = std::stoi(substr);
              },
      },
      PosSizeParser{
          .start = 11,
          .size = 3,
          .parser =
              [&tle](const std::string &substr) {
                tle.line_1.launch_number = std::stoi(substr);
              },
      },
      PosSizeParser{
          .start = 14,
          .size = 3,
          .parser =
              [&tle](const std::string &substr) {
                tle.line_1.launch_piece = substr;
              },
      },
      PosSizeParser{
          .start = 18,
          .size = 2,
          .parser =
              [&tle](const std::string &substr) {
                tle.line_1.epoch_year = std::stoi(substr);
              },
      },
      PosSizeParser{
          .start = 20,
          .size = 12,
          .parser =
              [&tle](const std::string &substr) {
                tle.line_1.epoch_day = std::stod(substr);
              },
      },
      PosSizeParser{
          .start = 33,
          .size = 10,
          .parser =
              [&tle](const std::string &substr) {
                tle.line_1.mean_motion_dot = std::stod(substr);
              },
      },
      PosSizeParser{
          .start = 44,
          .size = 8,
          .parser =
              [&tle](const std::string &substr) {
                tle.line_1.mean_motion_ddot = exponent_to_double(substr);
              },
      },
      PosSizeParser{
          .start = 53,
          .size = 8,
          .parser =
              [&tle](const std::string &substr) {
                tle.line_1.bstar_drag = exponent_to_double(substr);
              },
      },
      PosSizeParser{
          .start = 62,
          .size = 1,
          .parser =
              [&tle](const std::string &substr) {
                tle.line_1.ephemeris_type = std::stoi(substr);
              },
      },
      PosSizeParser{
          .start = 64,
          .size = 4,
          .parser =
              [&tle](const std::string &substr) {
                tle.line_1.element_number = std::stoi(substr);
              },
      },
      PosSizeParser{
          .start = 68,
          .size = 1,
          .parser =
              [&tle](const std::string &substr) {
                tle.line_1.checksum = std::stoi(substr);
              },
      },
  };

  for (auto &[start, size, parser] : line_1_parsers) {
    // we've checked line sizes so any errors indexing would be coding errors
    assert(0 <= start && start < line_1.size());
    assert(0 < size && size < line_1.size());
    assert(start + size <= line_1.size());

    auto substr = line_1.substr(start, size);
    try {
      parser(substr);
    } catch (std::exception &e) {
      throw MyException<std::string>(
          fmt::format(
              R"(Failed to parse TLE line 1 token, start={}, size={}, substr="{}")",
              start, size, substr),
          line_1);
    } catch (...) {
      throw MyException<std::string>(
          fmt::format(
              R"(Failed to parse TLE line 1 token, start={}, size={}, substr="{}")",
              start, size, substr),
          line_1);
    }
  }

  /// TODO is there a way to accomplish this without std::function?
  std::array<PosSizeParser, 10> line_2_parsers{
      PosSizeParser{
          .start = 0,
          .size = 1,
          .parser =
              [&tle](const std::string &substr) {
                tle.line_2.line_number = std::stoi(substr);
              },
      },
      PosSizeParser{
          .start = 2,
          .size = 5,
          .parser =
              [&tle](const std::string &substr) {
                tle.line_2.satellite_number = std::stoi(substr);
              },
      },
      PosSizeParser{
          .start = 8,
          .size = 8,
          .parser =
              [&tle](const std::string &substr) {
                tle.line_2.inclination = std::stod(substr);
              },
      },
      PosSizeParser{
          .start = 17,
          .size = 8,
          .parser =
              [&tle](const std::string &substr) {
                tle.line_2.raan = std::stod(substr);
              },
      },
      PosSizeParser{
          .start = 26,
          .size = 7,
          .parser =
              [&tle](const std::string &substr) {
                tle.line_2.eccentricity = std::stod("0." + substr);
              },
      },
      PosSizeParser{
          .start = 34,
          .size = 8,
          .parser =
              [&tle](const std::string &substr) {
                tle.line_2.argument_of_perigree = std::stod(substr);
              },
      },
      PosSizeParser{
          .start = 43,
          .size = 8,
          .parser =
              [&tle](const std::string &substr) {
                tle.line_2.mean_anomaly = std::stod(substr);
              },
      },
      PosSizeParser{
          .start = 52,
          .size = 11,
          .parser =
              [&tle](const std::string &substr) {
                tle.line_2.mean_motion = std::stod(substr);
              },
      },
      PosSizeParser{
          .start = 63,
          .size = 5,
          .parser =
              [&tle](const std::string &substr) {
                tle.line_2.rev_at_epoch = std::stoi(substr);
              },
      },
      PosSizeParser{
          .start = 68,
          .size = 1,
          .parser =
              [&tle](const std::string &substr) {
                tle.line_2.checksum = std::stoi(substr);
              },
      },
  };

  for (auto &[start, size, parser] : line_2_parsers) {
    // we've checked line sizes so any errors indexing would be coding errors
    assert(0 <= start && start < line_2.size());
    assert(0 < size && size < line_2.size());
    assert(start + size <= line_2.size());

    auto substr = line_2.substr(start, size);
    try {
      parser(substr);
    } catch (std::exception &e) {
      throw MyException<std::string>(
          fmt::format(
              R"(Failed to parse TLE line 2 token, start={}, size={}, substr="{}")",
              start, size, substr),
          line_2);
    } catch (...) {
      throw MyException<std::string>(
          fmt::format(
              R"(Failed to parse TLE line 2 token, start={}, size={}, substr="{}")",
              start, size, substr),
          line_2);
    }
  }

  // Line 1 parsed value checks
  if (tle.line_1.line_number != 1) {
    throw MyException<std::string>(
        fmt::format(R"(TLE line 1 contains invalid line number, value="{}")",
                    tle.line_1.line_number),
        line_1);
  }

  // only unclassified TLEs are in the public domain (that's all we have)
  // access to, so any other character is assumed to be an error
  if (tle.line_1.classification != 'U') {
    throw MyException<std::string>(
        fmt::format(
            R"(TLE line 1 contains invalid line number, value={}, expected="U")",
            tle.line_1.classification),
        line_1);
  }

  int line_1_computed_checksum = compute_checksum(line_1);
  if (tle.line_1.checksum != line_1_computed_checksum) {
    throw MyException<std::string>(
        fmt::format(
            R"(TLE line 1 contains invalid checksum, parsed={}, computed={})",
            tle.line_1.checksum, line_1_computed_checksum),
        line_1);
  }

  // Line 2 parsed value checks
  if (tle.line_2.line_number != 2) {
    throw MyException<std::string>(
        fmt::format(R"(TLE line 2 contains invalid line number, value="{}")",
                    tle.line_2.line_number),
        line_2);
  }

  if (auto msg =
          is_within_inclusive_domain(tle.line_2.inclination, 0.0, 180.0)) {
    throw MyException<std::string>(
        fmt::format(R"(TLE line 2 contains invalid inclination {})", *msg),
        line_2);
  }

  if (auto msg = is_within_inclusive_domain(tle.line_2.raan, 0.0, 360.0)) {
    throw MyException<std::string>(
        fmt::format(R"(TLE line 2 contains invalid RAAN {})", *msg), line_2);
  }

  if (auto msg =
          is_within_inclusive_domain(tle.line_2.eccentricity, 0.0, 1.0)) {
    throw MyException<std::string>(
        fmt::format(R"(TLE line 2 contains invalid eccentricity {})", *msg),
        line_2);
  }

  if (auto msg = is_within_inclusive_domain(tle.line_2.argument_of_perigree,
                                            0.0, 360.0)) {
    throw MyException<std::string>(
        fmt::format(R"(TLE line 2 contains invalid argument of perigree {})",
                    *msg),
        line_2);
  }

  if (auto msg =
          is_within_inclusive_domain(tle.line_2.mean_anomaly, 0.0, 360.0)) {
    throw MyException<std::string>(
        fmt::format(R"(TLE line 2 contains invalid mean anomaly {})", *msg),
        line_2);
  }

  int line_2_computed_checksum = compute_checksum(line_2);
  if (tle.line_2.checksum != line_2_computed_checksum) {
    throw MyException<std::string>(
        fmt::format(
            R"(TLE line 2 contains invalid checksum, parsed={}, computed={})",
            tle.line_2.checksum, line_2_computed_checksum),
        line_2);
  }

  // consistency checks between the two lines
  if (tle.line_1.satellite_number != tle.line_2.satellite_number) {
    throw MyException<std::string>(
        fmt::format(
            R"(parsed satellite numbers don't match between TLE lines, line_1_value={}, line_2_value={})",
            tle.line_1.satellite_number, tle.line_2.satellite_number),
        tle_str);
  }

  return tle;
}
}  // namespace eob
