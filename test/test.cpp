/**
 * @file
 *
 * Test the shared_string_view behavior.
 */

#include <sstream>
#include <string>
#include <gtest/gtest.h>
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
  // SSV created with a string object.
  {
    shared_string_view ssv{string("abc")};
    ASSERT_EQ(ssv.length(), 3);
  }
  // Copy constructor
  {
    shared_string_view ssv1{"abc"};
    shared_string_view ssv2{ssv1};
    ASSERT_EQ(string_view{ssv1}, string_view{ssv2});
  }
  // Move constructor
  {
    shared_string_view ssv1{"abc"};
    shared_string_view ssv2{move(ssv1)};
    ASSERT_NE(string_view{ssv1}, string_view{ssv2});
  }
}

TEST(Operator, string_stream) {
  {
    shared_string_view ssv{"abc 123"};
    ASSERT_EQ(string_view{ssv}, "abc 123"sv);
  }
  {
    shared_string_view ssv{"abc 123"};
    ASSERT_NE(string_view{ssv}, "abc 1234"sv);
  }
  {
    shared_string_view ssv{"abc 123"};
    ASSERT_NE(string_view{ssv}, ""sv);
  }
}

TEST(Operator, Extraction) {
  {
    shared_string_view ssv{"abc 123"};
    stringstream ss;
    ss << ssv;
    ASSERT_EQ(ss.str(), "abc 123");
  }
}

TEST(Operator, ThreeWayComparison) {
  shared_string_view ssv1{"abc 123"};
  shared_string_view ssv2{"abc 123"};
  shared_string_view ssv3{"abd 123"};

  // Test various comparison operators.
  ASSERT_EQ(ssv1, ssv2);
  ASSERT_NE(ssv1, ssv3);
  ASSERT_FALSE(ssv1 < ssv2);
  ASSERT_TRUE(ssv1 <= ssv2);
  ASSERT_FALSE(ssv1 > ssv2);
  ASSERT_TRUE(ssv1 >= ssv2);
  ASSERT_FALSE(ssv2 < ssv1);
  ASSERT_TRUE(ssv1 != ssv3);
  ASSERT_FALSE(ssv1 != ssv2);
  ASSERT_TRUE(ssv3 > ssv2);
  ASSERT_NE(ssv2, ssv3);

  // Force ssv2 and ssv3 to lose their target to make sure that this won't
  // cause a segfault.
  shared_string_view ssv4{move(ssv2)};
  shared_string_view ssv5{move(ssv3)};
  ASSERT_NE(ssv4, ssv5);
  ASSERT_NE(ssv1, ssv2);
  ASSERT_FALSE(ssv1 < ssv2);
  ASSERT_TRUE(ssv1 > ssv2);
  ASSERT_EQ(ssv2, ssv3);
}

int main(int argc, char** argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

