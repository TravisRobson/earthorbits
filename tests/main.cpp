#include <gtest/gtest.h>

#include <iostream>
#include <string>

#include "earthorbits/earthorbits.h"

TEST(EarthorbitTest, ParseTLES) {
  {
    std::string s =
        R"(1 25544U 98067A   24097.81509284  .00011771  00000-0  21418-3 0  9995
2 25544  51.6405 309.2692 0004792  43.0163  63.5300 15.49960977447473)";
    auto tle = eob::ParseTle(s);
    std::cout << tle << "\n";

    ASSERT_EQ(tle.line_1.line_number, 1);
    ASSERT_EQ(tle.line_1.satellite_number, 25544);
    ASSERT_EQ(tle.line_1.classification, 'U');
    ASSERT_EQ(tle.line_1.launch_year, 98);
    ASSERT_EQ(tle.line_1.launch_number, 67);
    ASSERT_EQ(tle.line_1.launch_piece, "A  ");
    ASSERT_EQ(tle.line_1.epoch_year, 24);
    ASSERT_FLOAT_EQ(tle.line_1.epoch_day, 097.81509284);
    ASSERT_FLOAT_EQ(tle.line_1.mean_motion_dot, .00011771);
    ASSERT_FLOAT_EQ(tle.line_1.mean_motion_ddot, 0.0);
    ASSERT_FLOAT_EQ(tle.line_1.bstar_drag, 0.21418e-3);
    ASSERT_EQ(tle.line_1.ephemeris_type, 0);
    ASSERT_EQ(tle.line_1.element_number, 999);
    ASSERT_EQ(tle.line_1.checksum, 5);

    ASSERT_EQ(tle.line_2.line_number, 2);
    ASSERT_EQ(tle.line_2.satellite_number, 25544);
    ASSERT_EQ(tle.line_2.inclination, 51.6405);
    ASSERT_EQ(tle.line_2.raan, 309.2692);
    ASSERT_EQ(tle.line_2.eccentricity, 0.0004792);
    ASSERT_EQ(tle.line_2.argument_of_perigree, 43.0163);
    ASSERT_EQ(tle.line_2.mean_anomaly, 63.5300);
    ASSERT_FLOAT_EQ(tle.line_2.mean_motion, 15.49960977);
    ASSERT_FLOAT_EQ(tle.line_2.rev_at_epoch, 44747);
    ASSERT_EQ(tle.line_2.checksum, 3);
  }

  std::cout << "\n";

  {
    std::string s =
        R"(1 25544U 98067A   24104.84924656  .00014577  00000-0  26139-3 0  9993
2 25544  51.6399 274.4116 0004733  65.7744  78.8036 15.50162147448561)";
    auto tle = eob::ParseTle(s);
    std::cout << tle << "\n";

    ASSERT_EQ(tle.line_1.line_number, 1);
    ASSERT_EQ(tle.line_1.satellite_number, 25544);
    ASSERT_EQ(tle.line_1.classification, 'U');
    ASSERT_EQ(tle.line_1.launch_year, 98);
    ASSERT_EQ(tle.line_1.launch_number, 67);
    ASSERT_EQ(tle.line_1.launch_piece, "A  ");
    ASSERT_EQ(tle.line_1.epoch_year, 24);
    ASSERT_FLOAT_EQ(tle.line_1.epoch_day, 104.84924656);
    ASSERT_FLOAT_EQ(tle.line_1.mean_motion_dot, .00014577);
    ASSERT_FLOAT_EQ(tle.line_1.mean_motion_ddot, 0.0);
    ASSERT_FLOAT_EQ(tle.line_1.bstar_drag, 0.26139e-3);
    ASSERT_EQ(tle.line_1.ephemeris_type, 0);
    ASSERT_EQ(tle.line_1.element_number, 999);
    ASSERT_EQ(tle.line_1.checksum, 3);

    ASSERT_EQ(tle.line_2.line_number, 2);
    ASSERT_EQ(tle.line_2.satellite_number, 25544);
    ASSERT_EQ(tle.line_2.inclination, 51.6399);
    ASSERT_EQ(tle.line_2.raan, 274.4116);
    ASSERT_EQ(tle.line_2.eccentricity, 0.0004733);
    ASSERT_EQ(tle.line_2.argument_of_perigree, 65.7744);
    ASSERT_EQ(tle.line_2.mean_anomaly, 78.8036);
    ASSERT_FLOAT_EQ(tle.line_2.mean_motion, 15.50162147);
    ASSERT_FLOAT_EQ(tle.line_2.rev_at_epoch, 44856);
    ASSERT_EQ(tle.line_2.checksum, 1);
  }

  std::cout << "\n";

  {
    std::string s =
        R"(1 25544U 98067A   08264.51782528 -.00002182  00000-0 -11606-4 0  2927
2 25544  51.6416 247.4627 0006703 130.5360 325.0288 15.72125391563537)";
    auto tle = eob::ParseTle(s);
    std::cout << tle << "\n";

    ASSERT_EQ(tle.line_1.line_number, 1);
    ASSERT_EQ(tle.line_1.satellite_number, 25544);
    ASSERT_EQ(tle.line_1.classification, 'U');
    ASSERT_EQ(tle.line_1.launch_year, 98);
    ASSERT_EQ(tle.line_1.launch_number, 67);
    ASSERT_EQ(tle.line_1.launch_piece, "A  ");
    ASSERT_EQ(tle.line_1.epoch_year, 8);
    ASSERT_FLOAT_EQ(tle.line_1.epoch_day, 264.51782528);
    ASSERT_FLOAT_EQ(tle.line_1.mean_motion_dot, -.00002182);
    ASSERT_FLOAT_EQ(tle.line_1.mean_motion_ddot, 0.0);
    ASSERT_FLOAT_EQ(tle.line_1.bstar_drag, -0.11606e-4);
    ASSERT_EQ(tle.line_1.ephemeris_type, 0);
    ASSERT_EQ(tle.line_1.element_number, 292);
    ASSERT_EQ(tle.line_1.checksum, 7);

    ASSERT_EQ(tle.line_2.line_number, 2);
    ASSERT_EQ(tle.line_2.satellite_number, 25544);
    ASSERT_EQ(tle.line_2.inclination, 51.6416);
    ASSERT_EQ(tle.line_2.raan, 247.4627);
    ASSERT_EQ(tle.line_2.eccentricity, 0.0006703);
    ASSERT_EQ(tle.line_2.argument_of_perigree, 130.5360);
    ASSERT_EQ(tle.line_2.mean_anomaly, 325.0288);
    ASSERT_FLOAT_EQ(tle.line_2.mean_motion, 15.72125391);
    ASSERT_FLOAT_EQ(tle.line_2.rev_at_epoch, 56353);
    ASSERT_EQ(tle.line_2.checksum, 7);
  }
}

TEST(EarthorbitTest, ParseInvalidTLES) {
  {  // Not enough characters, eliminated last character to test
    std::string s =
        R"(1 25544U 98067A   24097.81509284  .00011771  00000-0  21418-3 0  9995
2 25544  51.6405 309.2692 0004792  43.0163  63.5300 15.4996097744747)";

    ASSERT_THROW(eob::ParseTle(s), eob::EobError);
  }

  {  // expect linebreak precondition, replaced with space
    std::string s =
        R"(1 25544U 98067A   24097.81509284  .00011771  00000-0  21418-3 0  9995 2 25544  51.6405 309.2692 0004792  43.0163  63.5300 15.49960977447473)";

    ASSERT_THROW(eob::ParseTle(s), eob::EobError);
  }

  {  // expect linebreak precondition, replaced with space
    std::string s =
        R"(1 25544U 98067A   24097.81509284  .00011771  00000-0  21418-3 0  9995 2 25544  51.6405 309.2692 0004792  43.0163  63.5300 15.49960977447473)";

    ASSERT_THROW(eob::ParseTle(s), eob::EobError);
  }
}
