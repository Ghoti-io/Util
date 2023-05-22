/**
 * @file
 *
 * Test the general Wave server behavior.
 */

#include<string>
#include<gtest/gtest.h>
#include "shared_string_view.hpp"

using namespace std;
using namespace Ghoti;

TEST(Constructor, Length) {
  // SSV created with a C-string.
  {
    shared_string_view ssv{""};
    ASSERT_EQ(ssv.length(), 0);
  }
  {
    shared_string_view ssv{"a"};
    ASSERT_EQ(ssv.length(), 1);
  }
  {
    shared_string_view ssv{"abc"};
    ASSERT_EQ(ssv.length(), 3);
  }
  // SSV created with a C-string and explicit length.
  {
    shared_string_view ssv{"abc", 3};
    ASSERT_EQ(ssv.length(), 3);
  }
  {
    shared_string_view ssv{"abc", 1};
    ASSERT_EQ(ssv.length(), 1);
  }
}

int main(int argc, char** argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

