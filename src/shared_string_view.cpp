/**
 * @file
 *
 * Define the shared_string_view class.
 */

#include "shared_string_view.hpp"

using namespace std;
using namespace Ghoti;

shared_string_view::shared_string_view(const char * s) : target{make_shared<string>(s)}, start{0} {
  this->len = this->target->length();
}

shared_string_view::shared_string_view(const char * s, size_t len) : target{make_shared<string>(s, len)}, start{0} {
  this->len = this->target->length();
}

size_t shared_string_view::length() const {
  return this->len;
}

