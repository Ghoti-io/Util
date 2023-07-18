/**
 * @file
 *
 * Test the behavior of Ghoti.io Util HasParameters class..
 */

#include <gtest/gtest.h>
#include "util/anyMap.hpp"

using namespace std;
using namespace Ghoti::Util;

/// @cond HIDDEN_SYMBOLS
enum class Param {
  TEST1,
  TEST2,
  TEST3,
};
/// @endcond

TEST(AnyMap, BasicOperations) {
  AnyMap<Param> p{};
  // Verify empty construction.
  ASSERT_FALSE(p.getAny(Param::TEST1));
  ASSERT_FALSE(p.get<uint16_t>(Param::TEST1));
  ASSERT_FALSE(p.get<uint32_t>(Param::TEST1));
  ASSERT_ANY_THROW(*p.get<uint32_t>(Param::TEST1));
  ASSERT_ANY_THROW(*p.get<string>(Param::TEST1));

  // Verify that setting a value and then requesting it works as expected.
  ASSERT_NO_THROW(p.set(Param::TEST1, uint16_t{1}));
  ASSERT_TRUE(p.getAny(Param::TEST1));
  ASSERT_EQ(p.get<uint16_t>(Param::TEST1), 1);
  ASSERT_ANY_THROW(*p.get<uint32_t>(Param::TEST1));

  // Asking for a type on a value that does not exist (but not dereferencing it)
  // does not throw.
  ASSERT_NO_THROW(p.get<string>(Param::TEST2));

  // Verify that all other values are empty.
  ASSERT_FALSE(p.getAny(Param::TEST2));
  ASSERT_FALSE(p.get<uint16_t>(Param::TEST2));
  ASSERT_FALSE(p.get<string>(Param::TEST2));
  ASSERT_ANY_THROW(*p.get<uint32_t>(Param::TEST2));
}

TEST(HasParam, Set) {
  {
    AnyMap<Param> p{};

    // Verify that `p` contains what we expect.
    ASSERT_NO_THROW(p.set(Param::TEST1, uint32_t{1}));
    ASSERT_TRUE(p.getAny(Param::TEST1));
    ASSERT_EQ(*p.get<uint32_t>(Param::TEST1), 1);

    // Set TEST1 to a different type.
    p.set(Param::TEST1, bool{true});
    ASSERT_TRUE(p.getAny(Param::TEST1));
    ASSERT_TRUE(p.get<bool>(Param::TEST1));
    ASSERT_FALSE(p.get<uint32_t>(Param::TEST1));
    ASSERT_EQ(*p.get<bool>(Param::TEST1), true);
    ASSERT_ANY_THROW(*p.get<uint32_t>(Param::TEST1));
  }
  {
    // Verify that chaining works.
    AnyMap<Param> p{};
    ASSERT_FALSE(p.get<double>(Param::TEST1));
    ASSERT_FALSE(p.get<double>(Param::TEST2));
    ASSERT_FALSE(p.getAny(Param::TEST3));
    p.set(Param::TEST1, double{1})
      .set(Param::TEST2, double{2})
      .set(Param::TEST3, double{3});
    ASSERT_TRUE(p.get<double>(Param::TEST1));
    ASSERT_TRUE(p.get<double>(Param::TEST2));
    ASSERT_TRUE(p.get<double>(Param::TEST3));
    ASSERT_EQ(*p.get<double>(Param::TEST1), 1.);
    ASSERT_EQ(*p.get<double>(Param::TEST2), 2.);
    ASSERT_EQ(*p.get<double>(Param::TEST3), 3.);
  }
}

int main(int argc, char** argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

