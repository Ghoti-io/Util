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

namespace Ghoti::Test {
enum class ErrorCode {
  NOERROR = 0,
  ERROR1,
  ERROR2,
};

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
};

static const ErrorCategory constErrorCategory{};

std::error_code make_error_code(Ghoti::Test::ErrorCode ec) {
  return {static_cast<int>(ec), constErrorCategory};
}
}

namespace std {
template <>
struct is_error_code_enum<Ghoti::Test::ErrorCode> : true_type {};
}

using namespace Ghoti::Test;

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

