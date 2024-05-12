#pragma once

#include <math.h>

#include "constants.h"

namespace eob {
// [[nodiscard]] inline double wrap_to_2pi(double angle_radians) noexcept {
//   return angle_radians - pi2 * std::floor(angle_radians / pi2);
// }

[[nodiscard]] inline double wrap_to_86400(double seconds) noexcept {
  return seconds - seconds_per_day * std::floor(seconds / seconds_per_day);
}
}  // namespace eob
