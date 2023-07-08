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

// https://akrzemi1.wordpress.com/2017/07/12/your-own-error-code/
// https://akrzemi1.wordpress.com/2017/08/12/your-own-error-condition/

namespace Ghoti::Test {
enum class ErrorCode {
  NOERROR = 0,
  ERROR1 = 10,
  ERROR2,
  ERROR3 = 20,
};

enum class ErrorCondition {
  NOERROR = 0,
  CONDITION1,
  CONDITION2
};
}

namespace std {
template <>
struct is_error_code_enum<Ghoti::Test::ErrorCode> : true_type {};

template <>
struct is_error_condition_enum<Ghoti::Test::ErrorCondition> : true_type {};
}

namespace Ghoti::Test {

class ErrorCategory : public std::error_category {
  public:
  virtual const char* name() const noexcept {
    return "Ghoti::Test";
  }
  virtual string message(int ev) const {
    static unordered_map<ErrorCode, string> description {
      {ErrorCode::NOERROR, "No Error"},
      {ErrorCode::ERROR1, "Error 1"},
      {ErrorCode::ERROR2, "Error 2"},
    };
    auto ec = static_cast<ErrorCode>(ev);
    return description.contains(ec)
      ? description[ec]
      : "";
  }
  bool equivalent(const std::error_code & ec, int cond) const noexcept {
    const error_category & errCat = error_code{ErrorCode{}}.category();
    if (ec.category() == errCat) {
      return cond == (ec.value() / 10);
    }
    return false;
  }
  bool equivalent(int ec, const std::error_condition & cond) const noexcept {
    const error_category & errCat = error_condition{ErrorCondition{}}.category();
    if (cond.category() == errCat) {
      return (ec / 10) == cond.value();
    }
    return false;
  }
};

static const ErrorCategory constErrorCategory{};

std::error_code make_error_code(Ghoti::Test::ErrorCode ec) {
  return {static_cast<int>(ec), constErrorCategory};
}
std::error_condition make_error_condition(Ghoti::Test::ErrorCondition ec) {
  return {static_cast<int>(ec), constErrorCategory};
}
}

using namespace Ghoti::Test;

TEST(Error, BasicFunctionality) {
  ASSERT_EQ(ErrorCode::ERROR1, ErrorCode::ERROR1);
  ASSERT_NE(ErrorCode::ERROR1, ErrorCode::ERROR2);
  ASSERT_TRUE(ErrorCode::ERROR1 == ErrorCode::ERROR1);
  ASSERT_FALSE(ErrorCode::ERROR1 != ErrorCode::ERROR1);
  ASSERT_TRUE(error_code{ErrorCode::ERROR1} == ErrorCondition::CONDITION1);

  ASSERT_EQ(error_code{ErrorCode::ERROR1}, ErrorCondition::CONDITION1);
  ASSERT_EQ(error_code{ErrorCode::ERROR2}, ErrorCondition::CONDITION1);
  ASSERT_NE(error_code{ErrorCode::ERROR3}, ErrorCondition::CONDITION1);

  ASSERT_FALSE(error_code{ErrorCode::ERROR1} == ErrorCondition::CONDITION2);
  ASSERT_NE(error_code{ErrorCode::ERROR1}, ErrorCondition::CONDITION2);

  ASSERT_NE(error_code{ErrorCode::ERROR1}, ErrorCondition::CONDITION2);
  ASSERT_NE(error_code{ErrorCode::ERROR2}, ErrorCondition::CONDITION2);
  ASSERT_EQ(error_code{ErrorCode::ERROR3}, ErrorCondition::CONDITION2);
}

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
    std::error_code ec{ErrorCode::ERROR1};
    Util::ErrorOr<int> e{ec};
    ASSERT_FALSE(e);
    ASSERT_TRUE(e.error_code());
    ASSERT_EQ(e.error_code(), ErrorCode::ERROR1);
    ASSERT_ANY_THROW(*e);
  }
}

int main(int argc, char** argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

