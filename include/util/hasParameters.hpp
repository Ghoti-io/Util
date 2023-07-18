/**
 * @file
 *
 * Header file for declaring the hasParameters class.
 */

#ifndef GHOTI_UTIL_HASPARAMETERS_HPP
#define GHOTI_UTIL_HASPARAMETERS_HPP

#include <any>
#include <unordered_map>
#include "errorCode.hpp"
#include "errorOr.hpp"

namespace Ghoti::Util {

/**
 * A type alias for the structure that stores the settings map.
 */
template <typename T>
using ParameterMap = std::unordered_map<T, std::any>;

/**
 * Serves as a base class for any other class to have settings parameters.
 *
 * HasParameters is a templated utility class.  It's purpose is to associate
 * key/value pairs as settings, in which the keys are of an enum type and the
 * values may be of any type.
 *
 * In order to use this class, the programmer must supply one or two things.
 *   1. An `enum` or `enum class` type (with values defined, of course).
 *   2. (Optional) A function to supply default values.
 *
 * The default value function provided by the class will only provide an empty
 * object, but the programmer may create a subclass to override this behavior.
 *
 * A simple example of the usage of this class can be seen below:
 *
 * @code{cpp}
 * enum class Foo {
 *   GIMME_A_INT,
 *   GIMME_A_STRING,
 * };
 *
 * class FooWithDefaults : public Ghoti::Wave::HasParameters<Foo> {
 *   public:
 *   ErrorOr<any> getParameterDefault(const Foo & p) {
 *     if (p == Foo::GIMME_A_INT) {
 *       return int{1};
 *     }
 *     if (p == Foo::GIMME_A_STRING) {
 *       return string{"foo"};
 *     }
 *     return {};
 *   }
 * };
 * @endcode
 *
 * Alternate example of `FooWithDefaults` declaration:
 *
 * @code{cpp}
 * class FooWithDefaults : public Ghoti::Wave::HasParameters<Foo> {
 *   public:
 *   ErrorOr<any> getParameterDefault(const Foo & p) {
 *     unordered_map<Foo, ErrorOr<any>> defaults{
 *       {Foo::GIMME_A_INT, {int{1}}},
 *       {Foo::GIMME_A_STRING, {string{"foo"}}},
 *     };
 *     return defaults.contains(p) ? defaults[p] : {};
 *   }
 * };
 * @endcode
 *
 *
 * To Use it:
 *
 * @code{cpp}
 * int main() {
 *   FooWithDefaults f{};
 *   cout << *f.getParameter<uint32_t>(Foo::GIMME_A_INT) << endl;
 *   return 0;
 * }
 * @endcode
 *
 * Remember that `getParameter()` returns a `Ghoti::Util::ErrorOr<any>` and
 * `getParameters<type>()` returns a `Ghoti::Util::ErrorOr<type>`.  In either
 * case, you should verify that the value exists before dereferencing and
 * using the value.
 */
template <typename T>
class HasParameters {
  public:
  /**
   * The constructor.
   */
  HasParameters() : parameterValues{}, inheritFrom{} {}

  /**
   * Provide a default value for the provided parameter key.
   *
   * The default behavior of this function is to only return an error
   * value.  The intent is for this to be overridden by subclasses.
   *
   * @param parameter The parameter key to fetch.
   * @return The associated value.
   */
  virtual Ghoti::Util::ErrorOr<std::any> getParameterDefault([[maybe_unused]]const T & parameter) {
    return make_error_code(Ghoti::Util::ErrorCode::PARAMETER_NOT_FOUND);
  }

  /**
   * Gets the named parameter if it exists, checking locally first, then
   * checking the local defaults, then the inherited defaults (if set).
   *
   * @param parameter The parameter to get.
   * @return The parameter value if it exists, otherwise error.
   */
  virtual Ghoti::Util::ErrorOr<std::any> getParameterAny(const T & parameter) {
    // Search explicitly-set local values.
    if (this->parameterValues.contains(parameter)) {
      return this->parameterValues[parameter];
    }

    // Search default local values.
    auto localDefault = this->getParameterDefault(parameter);
    if (localDefault || !this->inheritFrom) {
      return localDefault;
    }

    // Search inherited values.
    return this->inheritFrom->getParameterAny(parameter);
  }


  /**
   * Get the parameter as a specified type.
   *
   * The result is returned as a `Ghoti::Util::ErrorOr`.  If there is no
   * parameter value, then the error value will be `false`.
   *
   * @param parameter The parameter value to get.
   * @return The parameter value if it exists, otherwise error.
   */
  template<class U>
  const Ghoti::Util::ErrorOr<U> getParameter(const T & parameter) {
    auto val = getParameterAny(parameter);
    if (val && (val->type() == typeid(U))) {
      return {std::any_cast<const U &>(*val)};
    }
    return make_error_code(Ghoti::Util::ErrorCode::PARAMETER_HAS_WRONG_TYPE);
  }

  /**
   * Set a parameter.
   *
   * @param parameter The parameter key to be set.
   * @param value The parameter value to be set.
   * @return The calling object, to allow for chaining.
   */
  virtual HasParameters & setParameter(const T & parameter, const std::any & value) {
    this->parameterValues[parameter] = value;
    return *this;
  }

  /**
   * Remove an explicitly-set parameter value.
   *
   * Default values are not altered.
   */
  virtual HasParameters & clearParameter(const T & parameter) {
    if (this->parameterValues.contains(parameter)) {
      this->parameterValues.erase(parameter);
    }
    return *this;
  }

  /**
   * Set a parent HasParameter object to inherit default values from.
   *
   * This parent object will be checked only if the current object does not
   * have a value explicitly set, or if the current object does not have its
   * own default set (by overriding HasParameters.getDefaultParameter()).
   *
   * @param inheritFrom The HasParameter object to inherit from.
   */
  virtual void setInheritFrom(HasParameters<T> * inheritFrom) noexcept {
    this->inheritFrom = inheritFrom;
  }

  /**
   * Get the current explicitly-set parameters and their values.
   *
   * @return The current explicitly-set parameters.
   */
  const ParameterMap<T> & getAllParameters() const {
    return this->parameterValues;
  }

  private:
  /**
   * Store explicitly set parameter key/value pairs.
   */
  ParameterMap<T> parameterValues;

  /**
   * Used to support an inheritance chain of where to look for default values.
   */
  HasParameters * inheritFrom;
};

}

#endif // GHOTI_UTIL_HASPARAMETERS_HPP

