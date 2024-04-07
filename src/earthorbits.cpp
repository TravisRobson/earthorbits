#include "earthorbits/earthorbits.h"

#include <cassert>
#include <cstddef>
#include <iostream>
#include <source_location>
#include <sstream>
#include <stdexcept>
#include <string>

#include <fmt/core.h>

namespace eob {
std::ostream& operator<<(std::ostream& os, const Tle &tle) { 
  return os << fmt::format(R"({{line_number={}, satellite_number={}, classification={}}})", 
    tle.line_1.line_number, tle.line_1.satellite_number, tle.line_1.classification);
}

constexpr int tle_line_size = 69;
Tle ParseTle(std::string &str) {
    // TODO turn this into a LOG statement
    std::cout << str << "\n";

    // two lines of 69 characters and a line break
    constexpr int expected_length = 2 * tle_line_size + 1;
    if (str.size() != expected_length) {
        auto loc = std::source_location::current();
        auto msg = fmt::format(R"({}:{} ss has invalid size, expected={}, value={}, tle_str="{}")", 
            loc.file_name(), loc.function_name(), expected_length, str.size(), str);
        throw EobError(msg);
    }

    constexpr size_t line_break_pos = 69;
    if (str[line_break_pos] != '\n') {
        auto loc = std::source_location::current();
        auto msg = fmt::format(R"({}:{} ss doesn't have linebreak at position={}, value={}, tle_str="{}")", 
            loc.file_name(), loc.function_name(), line_break_pos, str[line_break_pos], str);
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
            loc.file_name(), loc.function_name(), expected_length, str.size(), str);
        throw EobError(msg);
    }
    std::getline(ss, line_2, '\n');
    assert(line_2.size() == 69 && "TLE line 2 should be 69 characters");
    if (ss.fail()) {
        auto loc = std::source_location::current();
        auto msg = fmt::format(R"({}:{} failed to read TLE line 2, tle_str="{}")", 
            loc.file_name(), loc.function_name(), expected_length, str.size(), str);
        throw EobError(msg);
    }
    assert(ss.eof() && "Should only be two lines in TLE stringstream");

    std::cout << line_1 << "\n";
    try {
        size_t pos = 0;

        constexpr size_t ln_size = 1;
        tle.line_1.line_number = std::stoi(line_1.substr(pos, ln_size));
        pos += ln_size;

        ++pos; //  a space

        constexpr size_t cn_size = 5;
        tle.line_1.satellite_number = std::stoi(line_1.substr(pos, cn_size));
        pos += cn_size;

        constexpr size_t cl_size = 1;
        tle.line_1.classification = line_1.substr(pos, cl_size)[0];
        pos += cl_size;



    } catch (const std::out_of_range &e) {
        //  "if the converted value would fall out of the range of 
        //   the result type or if the underlying function"
        auto loc = std::source_location::current();
        auto msg = fmt::format(R"({}:{} failed to convert TLE field string, tle_str="{}")", 
            loc.file_name(), loc.function_name(), str);
        throw EobError(msg);
    } catch (const std::invalid_argument &e) {
        auto loc = std::source_location::current();
        auto msg = fmt::format(R"({}:{} failed to convert TLE field string, tle_str="{}")", 
            loc.file_name(), loc.function_name(), str);
        throw EobError(msg);
    }

    return tle;
}
} // namespace eob
