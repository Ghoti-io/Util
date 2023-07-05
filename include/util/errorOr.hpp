/**
 * @file
 * Header file containing the definitions of the ErrorOr class.
 */

#ifndef GHOTI_UTIL_ERROROR_HPP
#define GHOTI_UTIL_ERROROR_HPP

#include <system_error>
#include <variant>

namespace Ghoti::Util {
template<typename T>
class ErrorOr {
  public:
  /**
   * Default constructor.
   *
   * Will create an object containing the default value of type `T`.
   */
  ErrorOr() : value{T{}} {};

  /**
   * Will create an object containing the supplied `T` value.
   *
   * @param val The `T` type value with which to initialize the object.
   */
  ErrorOr(const T & val) : value{val} {};

  /**
   * Will create an object containing the supplied `std::error_code`.
   *
   * @param ec The `std::error_code` with which to initialize the object.
   */
  ErrorOr(const std::error_code & ec) : value{ec} {};

  /**
   * Return the `std::error_code` contained in the object.
   *
   * If no error code is present, then a default-constructed error code
   * will be returned.
   *
   * @return The `std::error_code` held by the object.
   */
  const std::error_code & error_code() const noexcept {
    return std::holds_alternative<std::error_code>(this->value)
      ? std::get<std::error_code>(this->value)
      : defaultError;
  }

  /**
   * Return the value as type `T`.
   *
   * It is up to the programmer to verify that the object does not hold
   * a `std::error_code`.
   *
   * @return The type-`T` value contained in the object.
   */
  const T & operator *() const {
    return std::get<T>(this->value);
  }

  const T * operator ->() const {
    return &std::get<T>(this->value);
  }


  /**
   * Indicate whether or not the non-error type is set.
   *
   * @return `true` if value is set, `false` if it is an error.
   *   This is the behavior of `std::optional` and `std::shared_ptr`.
   */
  operator bool() const noexcept {
    return std::holds_alternative<std::error_code>(this->value)
      ? false
      : true;
  }

  private:
  /**
   * Static helper which holds a default-constructed `std::error_code`, for
   * internal use.
   *
   * The "internal use" is that this allows the `error_code()` function to
   * be `noexcept` by returning this default error if no error is set.
   */
  inline static const std::error_code defaultError = {};

  /**
   * The actual value held by this object.
   */
  std::variant<std::error_code, T> value;
};
}

#endif // GHOTI_UTIL_ERROROR_HPP

