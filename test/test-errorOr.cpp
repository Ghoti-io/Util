/**
 * @file
 *
 * Test the shared_string_view behavior.
 */

#include <map>
#include <sstream>
#include <string>
#include <gtest/gtest.h>
#include "util/errorOr.hpp"

using namespace std;
using namespace Ghoti;

TEST(ErrorOr, Constructor) {
  {
    // Default Contructor should not be an error, and should have the default
    // value for that type
    Util::ErrorOr<int> e{};
    ASSERT_TRUE(e);
    ASSERT_FALSE(e.error_code());
    ASSERT_EQ(*e, int{});
  }
  {
    // Contructor with a specified value should not be an error.
    Util::ErrorOr<int> e{-10};
    ASSERT_TRUE(e);
    ASSERT_FALSE(e.error_code());
    ASSERT_EQ(*e, -10);
  }
  {
    // Contructor with a std::error_code should be fine, too
    error_code ec = make_error_code(errc::broken_pipe);
    Util::ErrorOr<int> e{ec};
    ASSERT_FALSE(e);
    ASSERT_TRUE(e.error_code());
    ASSERT_EQ(e.error_code(), errc::broken_pipe);
    ASSERT_ANY_THROW(*e);
  }
}

int main(int argc, char** argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

