#include "earthorbits/earthorbits.h"

#include <fmt/core.h>
#include <fmt/ostream.h>

#include <array>
#include <cassert>
#include <cstddef>
#include <iostream>
#include <limits>
#include <optional>
#include <source_location>
#include <sstream>
#include <stdexcept>
#include <string>

template <>
struct fmt::formatter<eob::Tle> : ostream_formatter {};

namespace eob {
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

namespace {
constexpr int tle_line_size = 69;

// https://codereview.stackexchange.com/a/39957
constexpr std::string_view tle_valid_chars =
    "ABCDEFGHIJKLMNOPQRSTUV+- 0123456789.";
bool is_valid(const std::string &str,
              const std::string_view &valid_chars) noexcept {
  std::array<bool, static_cast<size_t>(std::numeric_limits<char>::max())>
      mask{};
  for (char c : valid_chars) {
    mask[static_cast<size_t>(c)] = true;
  }

  return !std::any_of(str.begin(), str.end(), [&mask](char c) {
    return !mask[static_cast<size_t>(c)];
  });
}

/// @brief Convert TLE "exponential" string to double
/// @param sub_str TLE line substring containing the exponential
///
/// @pre Expects string to have at least 3 characters
///
/// @throws EobError if sub_str has unexpected format
/// @throw std::out_of_range from std::stod
/// @throws std::invalid_argument from std::stod
///
/// @return double, coverted from sub_str
double exponent_to_double(const std::string &sub_str) {
  assert(sub_str.size() > 3 && "exponent_to_double() string not large enough");

  double prefix_sign = 1.0;
  switch (sub_str[0]) {
    case '-':
      prefix_sign = -1.0;
      break;
    case '+':
      prefix_sign = 1.0;
      break;
    case ' ':
      prefix_sign = 1.0;
      break;
    default:
      auto loc = std::source_location::current();
      auto msg = fmt::format(R"({}:{} invalid exponential field, sub_str="{}")",
                             loc.file_name(), loc.function_name(), sub_str);
      throw EobError(msg);
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
      auto loc = std::source_location::current();
      auto msg = fmt::format(R"({}:{} invalid exponential field, sub_str="{}")",
                             loc.file_name(), loc.function_name(), sub_str);
      throw EobError(msg);
  }

  // skip first character when finding position of sign
  double left = std::stod("0." + sub_str.substr(1, sub_str.size() - 2));
  double right = std::stod(sub_str.substr(sub_str.size() - 1));

  return prefix_sign * left * std::pow(10.0, exp_sign * right);
}

/// The checksum is (Modulo 10) (Letters, blanks, periods, plus signs = 0; minus
/// signs = 1)
int compute_checksum(const std::string &line) {
  int sum = 0;
  for (char c : line.substr(0, line.size() - 1)) {
    if (std::isdigit(c)) {
      sum += static_cast<int>(c - '0');
    } else if (c == '-') {
      sum += 1;
    }
  }

  return sum % 10;
}

/// @brief Extract only filename from std::source_location::file_name()
/// @see https://stackoverflow.com/a/38237385
constexpr const char *filename(const char *path) {
  const char *file = path;
  while (*path) {
    if (*path++ == '/') {
      file = path;
    }
  }
  return file;
}

/// @brief  Utility which adds file and line number to message
std::string enrich_msg(
    std::string_view in,
    std::source_location loc = std::source_location::current()) {
  return fmt::format(R"({}:{} {})", filename(loc.file_name()), loc.line(), in);
}

/// @brief Check domain of parameter inclusive [lower_bound, upper_bound]
std::optional<std::string> check_inclusive_domain(
    double value, double lower_bound, double upper_bound,
    std::string_view description) {
  assert(lower_bound < upper_bound && "lower_bound must be < upper_bound");
  if (lower_bound <= value && value <= upper_bound) {
    return std::nullopt;
  }

  return std::optional<std::string>(enrich_msg(
      fmt::format(R"(invalid {}, value={}, lower_bound={}, upper_bound={})",
                  description, value, lower_bound, upper_bound)));
}
}  // namespace

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
/// TODO(tjr) should I make this return std::optional for failure modes?
/// TODO(tjr) can any of error logic code be consolidated?
Tle ParseTle(const std::string &tle_str) {
  // two lines of 69 characters and a line break
  constexpr int expected_length = 2 * tle_line_size + 1;
  if (tle_str.size() != expected_length) {
    auto msg = enrich_msg(fmt::format(
        R"(tle string has invalid size, expected={}, value={}, tle_str="{}")",
        expected_length, tle_str.size(), tle_str));
    throw EobError(msg);
  }

  constexpr size_t line_break_pos = 69;
  if (tle_str[line_break_pos] != '\n') {
    auto msg = enrich_msg(fmt::format(
        R"(tle string is invalid, linebreak not at expected position, )"
        R"(value={}, tle_str="{}")",
        tle_str.size(), tle_str));
    throw EobError(msg);
  }

  Tle tle;
  std::stringstream ss(tle_str);
  std::string line_1;
  std::string line_2;
  std::getline(ss, line_1, '\n');
  assert(line_1.size() == 69 && "TLE line 1 should be 69 characters");
  if (ss.fail()) {
    auto msg = enrich_msg(
        fmt::format(R"(failed to read TLE line 1, tle_str="{}")", tle_str));
    throw EobError(msg);
  }
  std::getline(ss, line_2, '\n');
  assert(line_2.size() == 69 && "TLE line 2 should be 69 characters");
  if (ss.fail()) {
    auto msg = enrich_msg(
        fmt::format(R"(failed to read TLE line 2, tle_str="{}")", tle_str));
    throw EobError(msg);
  }
  assert(ss.eof() && "Should only be two lines in TLE stringstream");

  if (!is_valid(line_1, tle_valid_chars)) {
    auto msg = enrich_msg(fmt::format(
        R"(TLE line 1 contains invalid characters, line_1="{}")", line_1));
    throw EobError(msg);
  }

  if (!is_valid(line_2, tle_valid_chars)) {
    auto msg = enrich_msg(fmt::format(
        R"(TLE line 2 contains invalid characters, line_2="{}")", line_2));
    throw EobError(msg);
  }

  try {
    size_t pos = 0;

    {
      constexpr size_t n = 1;
      tle.line_1.line_number = std::stoi(line_1.substr(pos, n));
      pos += n;
      assert(pos < tle_line_size && "ParseTle() position exceeded line size");
    }

    ++pos;  //  a space
    assert(pos < tle_line_size && "ParseTle() position exceeded line size");

    {
      constexpr size_t n = 5;
      tle.line_1.satellite_number = std::stoi(line_1.substr(pos, n));
      pos += n;
      assert(pos < tle_line_size && "ParseTle() position exceeded line size");
    }

    {
      constexpr size_t n = 1;
      tle.line_1.classification = line_1.substr(pos, n)[0];
      pos += n;
      assert(pos < tle_line_size && "ParseTle() position exceeded line size");
    }

    ++pos;  //  a space
    assert(pos < tle_line_size && "ParseTle() position exceeded line size");

    {
      constexpr size_t n = 2;
      tle.line_1.launch_year = std::stoi(line_1.substr(pos, n));
      pos += n;
      assert(pos < tle_line_size && "ParseTle() position exceeded line size");
    }

    {
      constexpr size_t n = 3;
      tle.line_1.launch_number = std::stoi(line_1.substr(pos, n));
      pos += n;
      assert(pos < tle_line_size && "ParseTle() position exceeded line size");
    }

    {
      constexpr size_t n = 3;
      tle.line_1.launch_piece = line_1.substr(pos, n);
      pos += n;
      assert(pos < tle_line_size && "ParseTle() position exceeded line size");
    }

    ++pos;  //  a space
    assert(pos < tle_line_size && "ParseTle() position exceeded line size");

    {
      constexpr size_t n = 2;
      tle.line_1.epoch_year = std::stoi(line_1.substr(pos, n));
      pos += n;
      assert(pos < tle_line_size && "ParseTle() position exceeded line size");
    }

    {
      constexpr size_t n = 12;
      tle.line_1.epoch_day = std::stod(line_1.substr(pos, n));
      pos += n;
      assert(pos < tle_line_size && "ParseTle() position exceeded line size");
    }

    ++pos;  //  a space
    assert(pos < tle_line_size && "ParseTle() position exceeded line size");

    {
      constexpr size_t n = 10;
      tle.line_1.mean_motion_dot = std::stod(line_1.substr(pos, n));
      pos += n;
      assert(pos < tle_line_size && "ParseTle() position exceeded line size");
    }

    ++pos;  //  a space
    assert(pos < tle_line_size && "ParseTle() position exceeded line size");

    {
      constexpr size_t n = 8;
      tle.line_1.mean_motion_ddot = exponent_to_double(line_1.substr(pos, n));
      pos += n;
      assert(pos < tle_line_size && "ParseTle() position exceeded line size");
    }

    ++pos;  //  a space
    assert(pos < tle_line_size && "ParseTle() position exceeded line size");

    {
      constexpr size_t n = 8;
      tle.line_1.bstar_drag = exponent_to_double(line_1.substr(pos, n));
      pos += n;
      assert(pos < tle_line_size && "ParseTle() position exceeded line size");
    }

    ++pos;  //  a space
    assert(pos < tle_line_size && "ParseTle() position exceeded line size");

    {
      constexpr size_t n = 1;
      tle.line_1.ephemeris_type = stoi(line_1.substr(pos, n));
      pos += n;
      assert(pos < tle_line_size && "ParseTle() position exceeded line size");
    }

    ++pos;  //  a space
    assert(pos < tle_line_size && "ParseTle() position exceeded line size");

    {
      constexpr size_t n = 4;
      tle.line_1.element_number = stoi(line_1.substr(pos, n));
      pos += n;
      assert(pos < tle_line_size && "ParseTle() position exceeded line size");
    }

    {
      constexpr size_t n = 1;
      tle.line_1.checksum = stoi(line_1.substr(pos, n));
    }

    pos = 0;  //  reset, moving onto line 2

    {
      constexpr size_t n = 1;
      tle.line_2.line_number = std::stoi(line_2.substr(pos, n));
      pos += n;
      assert(pos < tle_line_size && "ParseTle() position exceeded line size");
    }

    ++pos;  //  a space
    assert(pos < tle_line_size && "ParseTle() position exceeded line size");

    {
      constexpr size_t n = 5;
      tle.line_2.satellite_number = std::stoi(line_2.substr(pos, n));
      pos += n;
      assert(pos < tle_line_size && "ParseTle() position exceeded line size");
    }

    ++pos;  //  a space
    assert(pos < tle_line_size && "ParseTle() position exceeded line size");

    {
      constexpr size_t n = 8;
      tle.line_2.inclination = std::stod(line_2.substr(pos, n));
      pos += n;
      assert(pos < tle_line_size && "ParseTle() position exceeded line size");
    }

    ++pos;  //  a space
    assert(pos < tle_line_size && "ParseTle() position exceeded line size");

    {
      constexpr size_t n = 8;
      tle.line_2.raan = std::stod(line_2.substr(pos, n));
      pos += n;
      assert(pos < tle_line_size && "ParseTle() position exceeded line size");
    }

    ++pos;  //  a space
    assert(pos < tle_line_size && "ParseTle() position exceeded line size");

    {
      constexpr size_t n = 7;
      tle.line_2.eccentricity = std::stod("0." + line_2.substr(pos, n));
      pos += n;
      assert(pos < tle_line_size && "ParseTle() position exceeded line size");
    }

    ++pos;  //  a space
    assert(pos < tle_line_size && "ParseTle() position exceeded line size");

    {
      constexpr size_t n = 8;
      tle.line_2.argument_of_perigree = std::stod(line_2.substr(pos, n));
      pos += n;
      assert(pos < tle_line_size && "ParseTle() position exceeded line size");
    }

    ++pos;  //  a space
    assert(pos < tle_line_size && "ParseTle() position exceeded line size");

    {
      constexpr size_t n = 8;
      tle.line_2.mean_anomaly = std::stod(line_2.substr(pos, n));
      pos += n;
      assert(pos < tle_line_size && "ParseTle() position exceeded line size");
    }

    ++pos;  //  a space
    assert(pos < tle_line_size && "ParseTle() position exceeded line size");

    {
      constexpr size_t n = 11;
      tle.line_2.mean_motion = std::stod(line_2.substr(pos, n));
      pos += n;
      assert(pos < tle_line_size && "ParseTle() position exceeded line size");
    }

    {
      constexpr size_t n = 5;
      tle.line_2.rev_at_epoch = std::stoi(line_2.substr(pos, n));
      pos += n;
      assert(pos < tle_line_size && "ParseTle() position exceeded line size");
    }

    {
      constexpr size_t n = 1;
      tle.line_2.checksum = stoi(line_2.substr(pos, n));
    }

  } catch (const std::out_of_range &e) {
    //  "if the converted value would fall out of the range of
    //   the result type or if the underlying function"
    auto msg = enrich_msg(fmt::format(
        R"(std::out_of_range while parsing TLE, msg="{}", tle_str="{}")",
        e.what(), tle_str));
    throw EobError(msg);
  } catch (const std::invalid_argument &e) {
    auto msg = enrich_msg(fmt::format(
        R"(std::invalid_argument while parsing TLE, msg="{}", tle_str="{}")",
        e.what(), tle_str));
    throw EobError(msg);
  }

  // Line 1 parsed value checks
  if (tle.line_1.line_number != 1) {
    auto msg = enrich_msg(
        fmt::format(R"(TLE line 1 contains invalid line number, expected={}, )"
                    R"(value={}, line_1="{}")",
                    1, tle.line_1.line_number, line_1));
    throw EobError(msg);
  }

  // only unclassified TLEs are in the public domain (that's all we have)
  // access to, so any other character is assumed to be an error
  if (tle.line_1.classification != 'U') {
    auto msg = enrich_msg(fmt::format(
        R"(TLE line 1 contains invalid classification, expected={}, )"
        R"(value={}, line_1="{}")",
        'U', tle.line_1.classification, line_1));
    throw EobError(msg);
  }

  int line_1_computed_checksum = compute_checksum(line_1);
  if (tle.line_1.checksum != line_1_computed_checksum) {
    auto msg = enrich_msg(
        fmt::format(R"(TLE line 1 contains invalid checksum, computed={}, )"
                    R"(value={}, line_1="{}")",
                    line_1_computed_checksum, tle.line_1.checksum, line_1));
    throw EobError(msg);
  }

  // Line 2 parsed value checks
  if (tle.line_2.line_number != 2) {
    auto msg = enrich_msg(
        fmt::format(R"(TLE line 2 contains invalid line number, expected={}, )"
                    R"(value={}, line_2="{}")",
                    2, tle.line_2.line_number, line_2));
    throw EobError(msg);
  }

  if (auto msg = check_inclusive_domain(tle.line_2.inclination, 0.0, 180.0,
                                        "inclination")) {
    throw EobError(*msg);
  }

  if (auto msg = check_inclusive_domain(tle.line_2.raan, 0.0, 360.0, "RAAN")) {
    throw EobError(*msg);
  }

  if (auto msg = check_inclusive_domain(tle.line_2.eccentricity, 0.0, 1.0,
                                        "eccentricity")) {
    throw EobError(*msg);
  }

  if (auto msg = check_inclusive_domain(tle.line_2.argument_of_perigree, 0.0,
                                        360.0, "argument_of_perigree")) {
    throw EobError(*msg);
  }

  if (auto msg = check_inclusive_domain(tle.line_2.mean_anomaly, 0.0, 360.0,
                                        "mean_anomaly")) {
    throw EobError(*msg);
  }

  int line_2_computed_checksum = compute_checksum(line_2);
  if (tle.line_2.checksum != line_2_computed_checksum) {
    auto msg = enrich_msg(
        fmt::format(R"(TLE line 2 contains invalid checksum, computed={}, )"
                    R"(value={}, line_2="{}")",
                    line_2_computed_checksum, tle.line_2.checksum, line_2));
    throw EobError(msg);
  }

  // consistency checks between the two lines
  if (tle.line_1.satellite_number != tle.line_2.satellite_number) {
    auto loc = std::source_location::current();
    auto msg = fmt::format(
        R"({}:{} parsed satellite numbers don't match between TLE lines, line_1_value={}, line_2_value={})",
        loc.file_name(), loc.function_name(), tle.line_1.satellite_number,
        tle.line_2.satellite_number);
    throw EobError(msg);
  }

  return tle;
}
}  // namespace eob
