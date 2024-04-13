#include "earthorbits/earthorbits.h"

#include <fmt/core.h>
#include <fmt/ostream.h>

#include <array>
#include <cassert>
#include <cstddef>
#include <iostream>
#include <limits>
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
  return os << fmt::format(R"({{line_number={}}})", tle.line_2.line_number);
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
  for (char c : str) {
    if (!mask[static_cast<size_t>(c)]) {
      return false;
    }
  }
  return true;
}

double exponent_to_double(const std::string &sub_str) {
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
  switch (sub_str.end()[-2]) {
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
///
///   Line 1
///     [1] A = TLE line number {1}
///     [3, 7] BBBB = satellite number {25544}
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
///     [1] = TLE line number
///
///
/// The checksum is (Modulo 10) (Letters, blanks, periods, plus signs = 0; minus
/// signs = 1)
///
/// TODO(tjr) should I make this return std::optional for failure modes?
Tle ParseTle(const std::string &str) {
  // TODO turn this into a LOG statement
  std::cout << str << "\n";

  // two lines of 69 characters and a line break
  constexpr int expected_length = 2 * tle_line_size + 1;
  if (str.size() != expected_length) {
    auto loc = std::source_location::current();
    auto msg = fmt::format(
        R"({}:{} ss has invalid size, expected={}, value={}, tle_str="{}")",
        loc.file_name(), loc.function_name(), expected_length, str.size(), str);
    throw EobError(msg);
  }

  constexpr size_t line_break_pos = 69;
  if (str[line_break_pos] != '\n') {
    auto loc = std::source_location::current();
    auto msg = fmt::format(
        R"({}:{} ss doesn't have linebreak at position={}, value={}, tle_str="{}")",
        loc.file_name(), loc.function_name(), line_break_pos,
        str[line_break_pos], str);
    throw EobError(msg);
  }

  Tle tle;
  std::stringstream ss(str);
  std::string line_1;
  std::string line_2;
  std::getline(ss, line_1, '\n');
  assert(line_1.size() == 69 && "TLE line 1 should be 69 characters");
  if (ss.fail()) {
    auto loc = std::source_location::current();
    auto msg = fmt::format(
        R"({}:{} failed to read TLE line 1, size={}, tle_str="{}")",
        loc.file_name(), loc.function_name(), expected_length, str.size(), str);
    throw EobError(msg);
  }
  std::getline(ss, line_2, '\n');
  assert(line_2.size() == 69 && "TLE line 2 should be 69 characters");
  if (ss.fail()) {
    auto loc = std::source_location::current();
    auto msg = fmt::format(
        R"({}:{} failed to read TLE line 2, size={}, tle_str="{}")",
        loc.file_name(), loc.function_name(), expected_length, str.size(), str);
    throw EobError(msg);
  }
  assert(ss.eof() && "Should only be two lines in TLE stringstream");

  if (!is_valid(line_1, tle_valid_chars)) {
    auto loc = std::source_location::current();
    auto msg =
        fmt::format(R"({}:{} TLE line 1 has invalid characters, line_1="{}")",
                    loc.file_name(), loc.function_name(), line_1);
    throw EobError(msg);
  }

  if (!is_valid(line_2, tle_valid_chars)) {
    auto loc = std::source_location::current();
    auto msg =
        fmt::format(R"({}:{} TLE line 2 has invalid characters, line_2="{}")",
                    loc.file_name(), loc.function_name(), line_2);
    throw EobError(msg);
  }

  std::cout << line_1 << "\n";
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

      int computed_checksum = compute_checksum(line_1);
      if (tle.line_1.checksum != computed_checksum) {
        auto loc = std::source_location::current();
        auto msg = fmt::format(
            R"({}:{} TLE line 1 has invalid checksum, computed_checksum={}, parsed_checksum={}, line_1="{}")",
            loc.file_name(), loc.function_name(), computed_checksum,
            tle.line_1.checksum, line_2);
        throw EobError(msg);
      }
    }

  } catch (const std::out_of_range &e) {
    //  "if the converted value would fall out of the range of
    //   the result type or if the underlying function"
    auto loc = std::source_location::current();
    auto msg =
        fmt::format(R"({}:{} failed to convert TLE field string, tle_str="{}")",
                    loc.file_name(), loc.function_name(), str);
    throw EobError(msg);
  } catch (const std::invalid_argument &e) {
    auto loc = std::source_location::current();
    auto msg =
        fmt::format(R"({}:{} failed to convert TLE field string, tle_str="{}")",
                    loc.file_name(), loc.function_name(), str);
    throw EobError(msg);
  }

  // only unclassified TLEs are in the public domain (that's all we have)
  // access to, so any other character is assumed to be an error
  if (tle.line_1.classification != 'U') {
    auto loc = std::source_location::current();
    auto msg = fmt::format(
        R"({}:{} invalid classification, expected='U', value='{}', tle_str="{}")",
        loc.file_name(), loc.function_name(), tle.line_1.classification, str);
    throw EobError(msg);
  }

  return tle;
}
}  // namespace eob
