#include <gtest/gtest.h>

#include <iostream>
#include <string>

#include "earthorbits/earthorbits.h"

TEST(EarthorbitTest, ParseTLES) {
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
