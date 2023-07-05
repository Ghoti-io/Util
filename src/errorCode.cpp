/**
 * @file
 */

#include <unordered_map>
#include "util/errorCode.hpp"

using namespace std;
using namespace Ghoti;
using namespace Ghoti::Util;

// See http://blog.think-async.com/2010/04/system-error-support-in-c0x-part-5.html

const char * Util::ErrorCategory::name() const noexcept {
  return "Ghoti.io Util";
}

string Util::ErrorCategory::message(int ev) const {
  static unordered_map<ErrorCode, string> desc{
    {ErrorCode::PARAMETER_NOT_FOUND, "Parameter not found"},
    {ErrorCode::PARAMETER_HAS_WRONG_TYPE, "Parameter has wrong type"},
  };
  auto error{static_cast<ErrorCode>(ev)};
  if (desc.contains(error)) {
    return desc.at(error);
  }
  return ev ? "Unknown Error" : "";
}

bool Util::ErrorCategory::equivalent([[maybe_unused]] const std::error_code & code, [[maybe_unused]] int condition) const noexcept {
  return false;
}

const std::error_category & Util::getErrorCategory() {
  static ErrorCategory instance{};
  return instance;
}

std::error_code make_error_code(ErrorCode e) {
  return std::error_code(static_cast<int>(e), Util::getErrorCategory());
}

std::error_condition make_error_condition(ErrorCode e) {
  return std::error_condition(static_cast<int>(e), Util::getErrorCategory());
}

bool std::operator==(const std::error_code & lhs, ErrorCode rhs) {
  return lhs == ::make_error_code(rhs);
}

