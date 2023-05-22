/**
 * @file
 * Header file containing the definitions of the shared_string_view class.
 */

#ifndef SSV_HPP
#define SSV_HPP

#include <memory>
#include <ostream>
#include <string>
#include <string_view>

namespace Ghoti {
class shared_string_view {
  public:
  /**
   * Constructor
   *
   * @param s A C-string used to construct the string.
   */
  shared_string_view(const char * s);

  /**
   * Constructor
   *
   * @param s A C-string used to construct the string.
   * @param length The length of the C-string.
   */
  shared_string_view(const char * s, size_t length);

  /**
   * Constructor
   *
   * @param s A String object used to construct the string.
   */
  shared_string_view(const std::string & s);

  /**
   * Provides a string_view variant of the shared_string_view object.
   *
   * It is up to the programmer to ensure that the shared_string_view object
   * remains in scope while the string_view is in use.
   */
  operator std::string_view() const;

  /**
   * Return the length of the string represented by the view.
   *
   * The shared string may be longer, but this is the length of the substring
   * that this view represents.
   *
   * @return The length of the string represented by the view.
   */
  size_t length() const;

  private:
  /**
   * The shared target string pointed to by this object.
   */
  std::shared_ptr<std::string> target;

  /**
   * The offset into the shared target string at which this view begins.
   */
  size_t start;

  /**
   * The length of the view.
   */
  size_t len;
};
};

std::ostream & operator<<(std::ostream& out, const Ghoti::shared_string_view & ssv);

#endif // SSV_HPP

