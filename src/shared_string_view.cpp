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

shared_string_view::shared_string_view(const string & s) : target{make_shared<string>(s)}, start{0}, len{s.length()} {}

shared_string_view::operator string_view() const {
  return string_view{*this->target}.substr(this->start, this->len);
}

size_t shared_string_view::length() const {
  return this->len;
}

ostream & operator<<(ostream & out, const shared_string_view & ssv) {
  return out << string_view{ssv};
}

