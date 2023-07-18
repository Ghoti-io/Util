/**
 * @file
 *
 * Header file for declaring the hasParameters class.
 */

#ifndef GHOTI_UTIL_ANYMAP_HPP
#define GHOTI_UTIL_ANYMAP_HPP

#include <any>
#include <unordered_map>
#include "errorCode.hpp"
#include "errorOr.hpp"

namespace Ghoti::Util {

/**
 * A type alias for the structure that stores the settings map.
 */
template <typename T>
using AnyMapContents = std::unordered_map<T, std::any>;

/**
 * A wrapper class around a simple unordered map of arbitrary type (usually an
 * `enum`) to a `std::any` value.  Rather than returning the `any` value
 * directly, this class will return a `Ghoti::Util::ErrorOr` object which
 * contains an error message if the value requested does not exist or does not
 * match the requested type.
 *
 * AnyMap is a templated utility class.  It's purpose is to associate
 * key/value pairs (such as settings, or any other high-level language map or
 * associative array structure), in which the keys are (probably) of an `enum`
 * type and the values may be of `any` type.
 *
 * In order to use this class, the programmer must declare the `key` type.
 *
 * A simple example of the usage of this class can be seen below:
 *
 * @code{cpp}
 * enum class Foo {
 *   THING_A,
 *   THING_B,
 * };
 *
 * To Use it:
 *
 * @code{cpp}
 * int main() {
 *   AnyMap<Foo> f{};
 *   f.set(Foo::THING_A, uint32_t{42});
 *   cout << *f.get<uint32_t>(Foo::THING_A) << endl;
 *   return 0;
 * }
 * @endcode
 *
 * Remember that `get()` returns an `Ghoti::Util::ErrorOr<any>` and
 * `get<type>()` returns an `Ghoti::Util::ErrorOr<type>`.  In either case,
 * you should verify that the value exists before attempting to dereference
 * and use it.
 */
template <typename T>
class AnyMap {
  public:
  /**
   * The constructor.
   */
  AnyMap() : values{} {}

  /**
   * Gets the named key, if it exists.
   *
   * @param key The key whose value should be returned.
   * @return The key's value, if it exists.
   */
  const Ghoti::Util::ErrorOr<std::any> getAny(const T & key) {
    static Ghoti::Util::ErrorOr<std::any> error{make_error_code(Ghoti::Util::ErrorCode::ANYMAP_KEY_NOT_FOUND)};

    if (this->values.contains(key)) {
      return this->values[key];
    }

    return error;
  }


  /**
   * Get the key's value as a specified type.
   *
   * The result is returned as a `Ghoti::Util::ErrorOr`.
   *
   * @param key The key whose value should be returned.
   * @return The key's value, if it exists.
   */
  template<class U>
  const Ghoti::Util::ErrorOr<U> get(const T & key) {
    static Ghoti::Util::ErrorOr<U> error{make_error_code(Ghoti::Util::ErrorCode::ANYMAP_VALUE_HAS_WRONG_TYPE)};

    auto val = getAny(key);
    if (val && (val->type() == typeid(U))) {
      return {std::any_cast<const U &>(*val)};
    }

    return error;
  }

  /**
   * Set a value for the associated key.
   *
   * @param key The key to be set.
   * @param value The value to be set.
   * @return The calling object, to allow for chaining.
   */
  virtual AnyMap & set(const T & key, const std::any & value) {
    this->values[key] = value;
    return *this;
  }

  /**
   * Remove an explicitly-set key.
   *
   * @param key The key whose value should be cleared.
   */
  virtual AnyMap & clear(const T & key) {
    if (this->values.contains(key)) {
      this->values.erase(key);
    }
    return *this;
  }

  /**
   * Get the keys and their associated values.
   *
   * @return The keys and their associated values..
   */
  const AnyMapContents<T> & getAll() const {
    return this->values;
  }

  private:
  /**
   * Store explicitly set parameter key/value pairs.
   */
  AnyMapContents<T> values;
};

}

#endif // GHOTI_UTIL_ANYMAP_HPP

