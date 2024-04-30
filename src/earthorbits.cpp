#include "earthorbits/earthorbits.h"

#include "date/date.h"
// #include <fmt/core.h>
// #include <fmt/ostream.h>

#include <assert.h>

namespace eob {
/// @see https://celestrak.org/columns/v02n02/
///
/// θg(0h) = 24110s.54841 + 8640184s.812866 Tu + 0s.093104 Tu^2 - 6.2 × 10-6
/// Tu^3
double calc_sidereal_angle(double julian_days) {
  constexpr double denom_Tu = 1.0 / 36525.0;
  constexpr double denom_Tu_sq = denom_Tu * denom_Tu;
  constexpr double denom_Tu_cube = denom_Tu * denom_Tu * denom_Tu;

  constexpr double a = 8640184.812866 * denom_Tu;
  constexpr double b = 0.093104 * denom_Tu_sq;
  constexpr double c = 6.2e-6 * denom_Tu_cube;

  return 24110.54841 + julian_days * (a * +julian_days * (b - c * julian_days));
}

///
/// TODO this is really just a test of a structure I saw. I don't know that
/// Julian days
///   need to be positive, however, it could be useful for enforcing ranges
///   otherwise
///
/// @see https://godbolt.org/z/jPrEWxq63
///   shows how, when optimized, the same assembly instructions result
///
/// @see
/// https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#es106-dont-try-to-avoid-negative-values-by-using-unsigned
struct JulianDay {
  double val;
  explicit JulianDay(double x) : val{x} {
    assert(val >= 0 && "Julian days should be positive");
  }
  operator double() noexcept { return val; }
};

/// @see https://celestrak.org/columns/v02n02/
///
/// θg(0h) = 24110s.54841 + 8640184s.812866 Tu + 0s.093104 Tu^2 - 6.2 × 10-6
/// Tu^3
double calc_sidereal_angle(JulianDay jd) {
  constexpr double denom_Tu = 1.0 / 36525.0;
  constexpr double denom_Tu_sq = denom_Tu * denom_Tu;
  constexpr double denom_Tu_cube = denom_Tu * denom_Tu * denom_Tu;

  constexpr double a = 8640184.812866 * denom_Tu;
  constexpr double b = 0.093104 * denom_Tu_sq;
  constexpr double c = 6.2e-6 * denom_Tu_cube;

  return 24110.54841 + jd * (a * +jd * (b - c * jd));
}

constexpr double earth_rotation_rad_per_sec =
    7.29211510e-5;  // radians per second

using namespace date;

void foobar() { auto ymd = 2015_y / date::sep / 25; }
}  // namespace eob
