#pragma once

#include <iostream>

namespace eob {
struct TleLine1 {

};

struct TleLine2 {

};

/// @brief Two-line element
///
/// https://en.wikipedia.org/wiki/Two-line_element_set
/// https://celestrak.org/columns/v04n03/index.php#FAQ01
struct Tle {
  TleLine1 line_1;
  TleLine2 line_2;
};

Tle ParseTle(const std::iostream &i);
}  // namespace eob
