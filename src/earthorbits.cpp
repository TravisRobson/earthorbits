#include "earthorbits/earthorbits.h"

#include <fmt/core.h>
#include <fmt/ostream.h>

#include <cassert>
#include <cstddef>
#include <iostream>
#include <source_location>
#include <sstream>
#include <stdexcept>
#include <string>

template <>
struct fmt::formatter<eob::Tle> : ostream_formatter {};

namespace eob {
std::ostream &operator<<(std::ostream &os, const Tle &tle) {
  os << fmt::format(
      R"({{line_number={}, satellite_number={}, classification={}, launch_year={}, launch_piece={}, epoch_year={}, epoch_day={}, mean_motion_dot={}, mean_motion_ddot={}, bstar_drag={}, ephemeris_type={}, element_number={}, checksum={}}})",
      tle.line_1.line_number, tle.line_1.satellite_number,
      tle.line_1.classification, tle.line_1.launch_year,
      tle.line_1.launch_piece, tle.line_1.epoch_year, tle.line_1.epoch_day,
      tle.line_1.mean_motion_dot, tle.line_1.mean_motion_ddot,
      tle.line_1.bstar_drag, tle.line_1.ephemeris_type,
      tle.line_1.element_number, tle.line_1.checksum);
  os << ", ";
  return os << fmt::format(R"({{line_number={}}})", tle.line_2.line_number);
}

constexpr int tle_line_size = 69;

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
///     [10, 11] DD = International Designator (Last two digits of launch year)
///     {98} [12, 14] EEE = International Designator (Launch number of the year)
///     {067} [15, 17] FFF = International Designator (Piece of the launch) {A }
///     [19, 20] GG = Epoch Year (Last two digits of year) {24}
///     [21, 32] HHHHHHHHHHHH = Epoch (Day of the year and fractional portion of
///     the day) {097.81509284} [34, 43] IIIIIIIIII = First Time Derivative of
///     the Mean Motion { .00011771} [45, 52] JJJJJJJJ = Second Time Derivative
///     of Mean Motion (decimal point assumed) {JJJJJJJJ} [54, 61] KKKKKKKK =
///     BSTAR drag term (decimal point assumed) { 21418-3} [63] L = Ephemeris
///     type {0} [65, 68] MMMM = Element number { 999} [69] N = Checksum {9}
///
///   Line 2
///     [1] = TLE line number
///
///
/// The checksum is (Modulo 10) (Letters, blanks, periods, plus signs = 0; minus
/// signs = 1)
Tle ParseTle(std::string &str) {
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
    auto msg = fmt::format(R"({}:{} failed to read TLE line 1, tle_str="{}")",
                           loc.file_name(), loc.function_name(),
                           expected_length, str.size(), str);
    throw EobError(msg);
  }
  std::getline(ss, line_2, '\n');
  assert(line_2.size() == 69 && "TLE line 2 should be 69 characters");
  if (ss.fail()) {
    auto loc = std::source_location::current();
    auto msg = fmt::format(R"({}:{} failed to read TLE line 2, tle_str="{}")",
                           loc.file_name(), loc.function_name(),
                           expected_length, str.size(), str);
    throw EobError(msg);
  }
  assert(ss.eof() && "Should only be two lines in TLE stringstream");

  std::cout << line_1 << "\n";
  try {
    size_t pos = 0;

    constexpr size_t ln_size = 1;
    tle.line_1.line_number = std::stoi(line_1.substr(pos, ln_size));
    pos += ln_size;
    assert(pos < tle_line_size && "ParseTle() position exceeded line size");

    ++pos;  //  a space
    assert(pos < tle_line_size && "ParseTle() position exceeded line size");

    constexpr size_t cn_size = 5;
    tle.line_1.satellite_number = std::stoi(line_1.substr(pos, cn_size));
    pos += cn_size;
    assert(pos < tle_line_size && "ParseTle() position exceeded line size");

    constexpr size_t cl_size = 1;
    tle.line_1.classification = line_1.substr(pos, cl_size)[0];
    pos += cl_size;
    assert(pos < tle_line_size && "ParseTle() position exceeded line size");

    ++pos;  //  a space
    assert(pos < tle_line_size && "ParseTle() position exceeded line size");

    constexpr size_t ly_size = 2;
    tle.line_1.launch_year = std::stoi(line_1.substr(pos, ly_size));
    pos += ly_size;
    assert(pos < tle_line_size && "ParseTle() position exceeded line size");

    constexpr size_t lan_size = 3;
    tle.line_1.launch_number = std::stoi(line_1.substr(pos, lan_size));
    pos += lan_size;
    assert(pos < tle_line_size && "ParseTle() position exceeded line size");

    constexpr size_t lp_size = 3;
    tle.line_1.launch_piece = line_1.substr(pos, lp_size);
    pos += lp_size;
    assert(pos < tle_line_size && "ParseTle() position exceeded line size");

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
