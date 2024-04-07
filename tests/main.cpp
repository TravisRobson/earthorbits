#include "earthorbits/earthorbits.h"

#include <gtest/gtest.h>



TEST(EarthorbitTest, ParseTLES) {
    std::string s = R"(1 25544U 98067A   24097.81509284  .00011771  00000-0  21418-3 0  9995
2 25544  51.6405 309.2692 0004792  43.0163  63.5300 15.49960977447473)";
    auto tle = eob::ParseTle(s);
    std::cout << tle << "\n";
}


TEST(EarthorbitTest, ParseInvalidTLES) {
    { // Not enough characters, eliminated last character to test
        std::string s = R"(1 25544U 98067A   24097.81509284  .00011771  00000-0  21418-3 0  9995
2 25544  51.6405 309.2692 0004792  43.0163  63.5300 15.4996097744747)";

        ASSERT_THROW(eob::ParseTle(s), eob::EobError);
    }

    { // expect linebreak precondition, replaced with space
        std::string s = R"(1 25544U 98067A   24097.81509284  .00011771  00000-0  21418-3 0  9995 2 25544  51.6405 309.2692 0004792  43.0163  63.5300 15.49960977447473)";

        ASSERT_THROW(eob::ParseTle(s), eob::EobError);
    }
}
