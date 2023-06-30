/**
 * @file
 *
 * Define the shared_string_view class.
 */

#include "util/shared_string_view.hpp"
#include <iostream>

using namespace std;
using namespace Ghoti;

shared_string_view::shared_string_view() : target{make_shared<string>("")}, start{0}, len{0} {}

shared_string_view::shared_string_view(const char * s) : target{make_shared<string>(s)}, start{0} {
  this->len = this->target->length();
}

shared_string_view::shared_string_view(const char * s, size_t len) : target{make_shared<string>(s, len)}, start{0} {
  this->len = this->target->length();
}

shared_string_view::shared_string_view([[maybe_unused]] bool ignore) : target{nullptr}, start{0}, len{0} {}

shared_string_view::shared_string_view(const string & s) : target{make_shared<string>(s)}, start{0}, len{s.length()} {}

shared_string_view::operator string_view() const {
  if (this->target) {
    return string_view{*this->target}.substr(this->start, this->len);
  }
  return ""sv;
}

size_t shared_string_view::length() const {
  return this->len;
}

shared_string_view shared_string_view::substr(size_t offset, size_t length) {
  auto copy{*this};
  copy.start = min(this->start + offset, this->start + this->len);
  copy.len = min(this->len - (copy.start - this->start), length);
  return copy;
}

bool shared_string_view::operator==(const shared_string_view & ssv) const {
  return this->target && ssv.target
    ? std::string_view{*this} == std::string_view{ssv}
    : this->target || ssv.target
      ? false
      : true;
}

std::weak_ordering shared_string_view::operator<=> (const shared_string_view & ssv) const {
  return this->target && ssv.target
    ? std::string_view{*this} <=> std::string_view{ssv}
    : this->target || ssv.target
      ? this->target
        ? std::weak_ordering::greater
        : std::weak_ordering::less
      : std::weak_ordering::equivalent;
}

shared_string_view & shared_string_view::operator+=(const shared_string_view & rhs) {
  if ((this->start == 0) && (this->len == this->target->length())) {
    // Allow the string itself to be modified if and only if this view points
    // to the entirety of the target string.
    *this->target += ((rhs.start == 0) && (rhs.len == rhs.target->length()))
      ? *rhs.target
      : rhs.target->substr(rhs.start, rhs.len);
  }
  else {
    // Otherwise, make a copy of the important part of the original target
    // string, and append the new string to it.
    this->target = make_shared<string>(this->target->substr(this->start, this->len) + (
      ((rhs.start == 0) && (rhs.len == rhs.target->length()))
        ? *rhs.target
        : rhs.target->substr(rhs.start, rhs.len)));
    this->start = 0;
  }
  this->len += rhs.length();
  return *this;
}

shared_string_view & shared_string_view::operator+=(char rhs) {
  if ((this->start == 0) && (this->len == this->target->length())) {
    // Allow the string itself to be modified if and only if this view points
    // to the entirety of the target string.
    *this->target += rhs;
  }
  else {
    // Otherwise, make a copy of the important part of the original target
    // string, and append the new string to it.
    this->target = make_shared<string>(this->target->substr(this->start, this->len) + rhs);
    this->start = 0;
  }
  ++this->len;
  return *this;
}

shared_string_view shared_string_view::operator+(const shared_string_view & rhs) const {
  shared_string_view next{false};

  // If the view includes the entire target string, then we don't have to
  // use substr(), which avoids making a copy of the string.
  next.target = make_shared<string>(
    ((this->start == 0) && (this->len == this->target->length())
      ? *this->target
      : this->target->substr(this->start, this->len)) + 
    ((rhs.start == 0) && (rhs.len == rhs.target->length())
      ? *rhs.target
      : rhs.target->substr(rhs.start, rhs.len)
    ));

  next.len = next.target->length();
  return next;
}

char shared_string_view::operator[](size_t pos) const {
  return (*this->target)[pos];
}

string_view::const_iterator shared_string_view::begin() const {
  return string_view{*this}.begin();
}

string_view::const_iterator shared_string_view::end() const {
  return string_view{*this}.end();
}

string_view::const_reverse_iterator shared_string_view::rbegin() const {
  return string_view{*this}.rbegin();
}

string_view::const_reverse_iterator shared_string_view::rend() const {
  return string_view{*this}.rend();
}

ostream & Ghoti::operator<<(ostream & out, const shared_string_view & ssv) {
  return out << string_view{ssv};
}

std::size_t std::hash<Ghoti::shared_string_view>::operator()(const Ghoti::shared_string_view & ssv) const noexcept {
  return hash<string_view>{}(ssv);
}

