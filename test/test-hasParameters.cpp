/**
 * @file
 *
 * Test the behavior of Ghoti.io Util HasParameters class..
 */

#include <gtest/gtest.h>
#include "util/hasParameters.hpp"

using namespace std;
using namespace Ghoti::Util;

/// @cond HIDDEN_SYMBOLS
enum class Param {
  TEST1,
  TEST2,
  TEST3,
};

class HasParam : public HasParameters<Param> {
  public:
  ErrorOr<any> getParameterDefault(const Param & p) override {
    if (p == Param::TEST1) {
      return {uint32_t{1}};
    }
    if (p == Param::TEST2) {
      return {string{"foo"}};
    }
    return make_error_code(ErrorCode::PARAMETER_NOT_FOUND);
  }
};

class HasParamNoDefaults : public HasParameters<Param> {};

/// @endcond

TEST(HasParameters, WithDefaults) {
  HasParam p{};
  // Verify default values exist, are of the correct type, and can be accessed
  // in various ways.
  ASSERT_TRUE(p.getParameterAny(Param::TEST1));
  ASSERT_NO_THROW(*p.getParameterAny(Param::TEST1));
  ASSERT_EQ(any_cast<uint32_t>(*p.getParameterAny(Param::TEST1)), 1);
  ASSERT_EQ(*p.getParameter<uint32_t>(Param::TEST1), 1);
  ASSERT_FALSE(p.getParameter<uint16_t>(Param::TEST1));
  ASSERT_TRUE(p.getParameter<uint32_t>(Param::TEST1));
  // Trying to dereference the wrong type will throw an exception.
  ASSERT_ANY_THROW(*p.getParameter<string>(Param::TEST1));
  // Asking for the wrong type (but not dereferencing) does not throw.
  ASSERT_NO_THROW(p.getParameter<string>(Param::TEST1));

  ASSERT_TRUE(p.getParameterAny(Param::TEST2));
  ASSERT_FALSE(p.getParameter<uint16_t>(Param::TEST2));
  ASSERT_TRUE(p.getParameter<string>(Param::TEST2));
  ASSERT_EQ(*p.getParameter<string>(Param::TEST2), "foo");
  ASSERT_NO_THROW(p.getParameter<uint32_t>(Param::TEST2));
  ASSERT_ANY_THROW(*p.getParameter<uint32_t>(Param::TEST2));

  // Verify that a default value does not exist.
  ASSERT_FALSE(p.getParameterAny(Param::TEST3));
  ASSERT_FALSE(p.getParameter<uint16_t>(Param::TEST3));
  ASSERT_FALSE(p.getParameter<string>(Param::TEST3));
  ASSERT_NO_THROW(p.getParameter<string>(Param::TEST3));
  ASSERT_ANY_THROW(*p.getParameter<string>(Param::TEST3));
}

TEST(HasParameters, NoDefaults) {
  HasParamNoDefaults p{};
  // Verify default values exist and are of the correct type.
  ASSERT_FALSE(p.getParameterAny(Param::TEST1));
  ASSERT_FALSE(p.getParameter<uint16_t>(Param::TEST1));
  ASSERT_FALSE(p.getParameter<uint32_t>(Param::TEST1));
  ASSERT_ANY_THROW(*p.getParameter<uint32_t>(Param::TEST1));
  ASSERT_ANY_THROW(*p.getParameter<string>(Param::TEST1));
  // Asking for a type on a value that does not exist (but not dereferencing it)
  // does not throw.
  ASSERT_NO_THROW(p.getParameter<string>(Param::TEST1));

  ASSERT_FALSE(p.getParameterAny(Param::TEST2));
  ASSERT_FALSE(p.getParameter<uint16_t>(Param::TEST2));
  ASSERT_FALSE(p.getParameter<string>(Param::TEST2));
  ASSERT_ANY_THROW(*p.getParameter<uint32_t>(Param::TEST2));

  // Verify that a default value does not exist.
  ASSERT_FALSE(p.getParameterAny(Param::TEST3));
  ASSERT_FALSE(p.getParameter<uint16_t>(Param::TEST3));
  ASSERT_FALSE(p.getParameter<string>(Param::TEST3));
  ASSERT_ANY_THROW(*p.getParameter<uint32_t>(Param::TEST3));
}

TEST(HasParam, Set) {
  {
    HasParam p{};
    HasParam p2{};

    // Verify that `p` contains what we expect.
    ASSERT_TRUE(p.getParameterAny(Param::TEST1));
    ASSERT_EQ(*p.getParameter<uint32_t>(Param::TEST1), 1);

    // Set TEST1 to a different type.
    p.setParameter(Param::TEST1, bool{true});
    ASSERT_TRUE(p.getParameterAny(Param::TEST1));
    ASSERT_TRUE(p.getParameter<bool>(Param::TEST1));
    ASSERT_FALSE(p.getParameter<uint32_t>(Param::TEST1));
    ASSERT_EQ(*p.getParameter<bool>(Param::TEST1), true);

    // Validate that the default values are not changed for other instances.
    ASSERT_EQ(*p2.getParameter<uint32_t>(Param::TEST1), 1);
  }
  {
    // Verify that chaining works.
    HasParam p{};
    ASSERT_TRUE(p.getParameter<uint32_t>(Param::TEST1));
    ASSERT_TRUE(p.getParameter<string>(Param::TEST2));
    ASSERT_FALSE(p.getParameterAny(Param::TEST3));
    p.setParameter(Param::TEST1, double{1})
      .setParameter(Param::TEST2, double{2})
      .setParameter(Param::TEST3, double{3});
    ASSERT_TRUE(p.getParameter<double>(Param::TEST1));
    ASSERT_TRUE(p.getParameter<double>(Param::TEST2));
    ASSERT_TRUE(p.getParameter<double>(Param::TEST3));
    ASSERT_EQ(*p.getParameter<double>(Param::TEST1), 1.);
    ASSERT_EQ(*p.getParameter<double>(Param::TEST2), 2.);
    ASSERT_EQ(*p.getParameter<double>(Param::TEST3), 3.);
  }
}

TEST(HasParam, Inheritance) {
  {
    HasParam p1{};
    HasParamNoDefaults p2{};

    // Validate default behavior.
    ASSERT_TRUE(p1.getParameterAny(Param::TEST1));
    ASSERT_FALSE(p2.getParameterAny(Param::TEST1));

    // Validate setting inheritance.
    p2.setInheritFrom(&p1);
    ASSERT_TRUE(p2.getParameterAny(Param::TEST1));
    ASSERT_EQ(*p2.getParameter<uint32_t>(Param::TEST1), 1.);
    ASSERT_ANY_THROW(*p2.getParameter<double>(Param::TEST1));

    // Clear the inheritance.
    p2.setInheritFrom(nullptr);
    ASSERT_FALSE(p2.getParameterAny(Param::TEST1));
    ASSERT_ANY_THROW(*p2.getParameter<uint32_t>(Param::TEST1));
  }
  {
    // Validate that explicitly set local values are respected.
    HasParam p1{};
    HasParamNoDefaults p2{};

    p2.setParameter(Param::TEST1, uint32_t{42});
    p1.setInheritFrom(&p2);

    ASSERT_EQ(*p1.getParameter<uint32_t>(Param::TEST1), 1);
    ASSERT_EQ(*p2.getParameter<uint32_t>(Param::TEST1), 42);
  }
}

TEST(HasParam, ClearParamValue) {
  {
    // Validate that a cleared parameter reverts to the default.
    HasParam p{};
    ASSERT_EQ(*p.getParameter<uint32_t>(Param::TEST1), 1);
    p.setParameter(Param::TEST1, string{"howdy"});
    ASSERT_EQ(*p.getParameter<string>(Param::TEST1), "howdy");
    p.clearParameter(Param::TEST1);
    ASSERT_EQ(*p.getParameter<uint32_t>(Param::TEST1), 1);
  }
  {
    // Validate that a cleared parameter reverts to nothing if no default
    // exists.
    HasParam p{};
    ASSERT_FALSE(p.getParameterAny(Param::TEST3));
    ASSERT_ANY_THROW(*p.getParameter<uint32_t>(Param::TEST3));
    p.setParameter(Param::TEST1, string{"howdy"});
    ASSERT_EQ(*p.getParameter<string>(Param::TEST1), "howdy");
    p.clearParameter(Param::TEST1);
    ASSERT_FALSE(p.getParameterAny(Param::TEST3));
    ASSERT_ANY_THROW(*p.getParameter<uint32_t>(Param::TEST3));
  }
}

int main(int argc, char** argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

