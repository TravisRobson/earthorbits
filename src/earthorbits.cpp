#include "earthorbits/earthorbits.h"

#include <fmt/core.h>

#include "constants.h"
#include "date/date.h"
#include "eobmath.h"

namespace eob {
namespace {
/// @brief Compute sidereal angle at 0h UTC for Greenwich mean time
///
/// θg(0h) = 24110s.54841 + 8640184s.812866 Tu + 0s.093104 Tu^2 - 6.2x10-6 Tu^3
/// "s" above are seconds, i.e. sidereal time, not angle (radians, degrees)
///
/// I believe we can get away without using Julian days as long
/// as we are in modern enough times?
///
/// @see https://en.wikipedia.org/wiki/Sidereal_time
/// @see https://celestrak.org/columns/v02n01/
/// @see https://celestrak.org/columns/v02n02/
/// Verified with the following calculator:
/// @see https://aa.usno.navy.mil/data/siderealtime
/// To get current julian date/day:
/// @see https://aa.usno.navy.mil/data/JulianDate
///
/// Tu variable below is really days since Jan 1, 2000 UT1, however,
/// I am using UTC time.
/// TODO Investigate importance of UT1-UTC corrections
/// @see
/// https://crf.usno.navy.mil/global-solutions-eop?pageid=vlbi-analysis-center
/// @see https://celestrak.org/SpaceData/
///
/// Mean time, as opposed to apparent time, does not account for the corrections
/// needed to to Earth's nutation. Precession is accounted for however.
/// @see https://lweb.cfa.harvard.edu/~jzhao/times.html
///
/// @returns Greenwich mean sidereal angle, seconds, at midnight UT1 (UTC?)
[[nodiscard]] double calc_gmst_0h(
    const std::chrono::time_point<std::chrono::system_clock> &tp) noexcept {
  using namespace date;
  using namespace std::chrono;

  constexpr system_clock::time_point T0 =
      date::sys_days(2000_y / date::jan / 1) + 12h;
  std::chrono::duration<double, std::chrono::days::period> delta_days =
      (tp - T0);

  double Tu = delta_days.count() / 36525.0;

  constexpr double a = 8640184.812866;
  constexpr double b = 0.093104;
  constexpr double c = 6.2e-6;

  return 24110.54841 + Tu * (a + Tu * (b - c * Tu));
}
}  // anonymous namespace

[[nodiscard]] std::string to_string(
    const std::chrono::time_point<std::chrono::system_clock> &tp) {
  using namespace std::chrono;

  std::array<char, std::size("yyyy-mm-ddTHH:MM:SS")> ymd_hms;
  auto tc = system_clock::to_time_t(tp);
  std::strftime(ymd_hms.data(), ymd_hms.size(), "%FT%T", std::gmtime(&tc));
  auto ms = duration_cast<milliseconds>(tp.time_since_epoch()).count() % 1000;
  return fmt::format("{}.{:03d}Z", ymd_hms.data(), ms);
}

[[nodiscard]] eob_seconds calc_gmst(
    const std::chrono::time_point<std::chrono::system_clock> &tp) noexcept {
  auto tp_0h = std::chrono::floor<std::chrono::days>(tp);
  auto gmst_0h = calc_gmst_0h(tp_0h);

  std::chrono::duration<double, std::chrono::seconds::period> delta_s =
      (tp - tp_0h);

  /// @brief Rotation of Earth, radians per second
  /// @see https://celestrak.org/columns/v02n01/
  constexpr double earth_rotation_rad_per_s = 7.29211510e-5;
  constexpr double earth_rotation =
      earth_rotation_rad_per_s * seconds_per_day / pi2;

  return eob_seconds{wrap_to_86400(gmst_0h + earth_rotation * delta_s.count())};
}
}  // namespace eob
