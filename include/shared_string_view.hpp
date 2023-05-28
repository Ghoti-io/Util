/**
 * @file
 * Header file containing the definitions of the shared_string_view class.
 */

#ifndef SSV_HPP
#define SSV_HPP

#include <compare>
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

  /**
   * Calculate a substring based on the current shared_string_view.
   *
   * If the substring is out of range, then an empty view will be provided.  If
   * the substring length requested is greater than what is available, then
   * the returned substring will contain as many characters as possible, within
   * the limits of the parent string view range.
   *
   * @param offset The 0-based offset from which the substring should start.
   * @param length The length of the substring desired.
   * @return A new shared_string_view of the requested substring.
   */
  shared_string_view substr(size_t offset, size_t length);

  /**
   * Compare two shared_string_view objects.
   *
   * @param ssv The right hand side operator.
   * @return True if the objects have equivalent values, false otherwise.
   */
  bool operator==(const shared_string_view & ssv) const;

  /**
   * Perform a three-way comparison on two shared_string_view objects.
   *
   * @param ssv The right hand side operator.
   * @return A weak ordering indicator of the two objects.
   */
  std::weak_ordering operator<=> (const shared_string_view & ssv) const;

  /**
   * Perform a concatenation of the supplied string to the existing string
   * view object and apply it to the existing string view object.
   *
   * If the target string can be appended to safely, then that will be done.
   * Otherwise, a new internal string will be created.
   *
   * Because this may modify the target string, all previously-provided
   * `std::string_view` references will be invalidated.  This is similar to the
   * behavior of `std::string.cstr()`, in which modifying the string will
   * invalidate the c-string pointer.
   *
   * @param rhs A string to be appended to the `shared_string_view` object.
   * @return The amended `shared_string_view` resulting from the concatenation.
   */
  shared_string_view & operator+=(const std::string & rhs);

  /**
   * Perform a concatenation of the supplied string to the existing string
   * view object.  Return a new string view.
   *
   * @param rhs A string to be appended to the `shared_string_view` object.
   * @return The new `shared_string_view` resulting from the concatenation.
   */
  shared_string_view operator+(const std::string & rhs);

  /**
   * Provide an iterator from the beginning of the view.
   *
   * @return A forward iterator.
   */
  std::string_view::const_iterator begin() const;

  /**
   * Provide an iterator pointing to the end of the view.
   *
   * @return An ending iterator.
   */
  std::string_view::const_iterator end() const;

  /**
   * Provide a reverse iterator from the end of the view.
   *
   * @return A reverse iterator.
   */
  std::string_view::const_reverse_iterator rbegin() const;

  /**
   * Provide a reverse iterator pointing to the beginning of the view.
   *
   * @return An reverse ending iterator.
   */
  std::string_view::const_reverse_iterator rend() const;

  /**
   * Perform an index operation into the target string.
   *
   * @param pos The 0-based index position.
   * @return The character at the position requested.
   */
  char operator[](size_t pos) const;

  private:
  /**
   * Private constructor.
   *
   * This constructor is private because it will create an object whose target
   * is not initialized, which should not be done in general.
   */
  shared_string_view();

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

/**
 * Insertion operator.
 *
 * @param out The output stream to be written to.
 * @param ssv The `shared_string_view` to be inserted into the stream.
 * @return The output stream.
 */
std::ostream & operator<<(std::ostream& out, const Ghoti::shared_string_view & ssv);

};

/**
 * Hashing function, consistent with `std::string_view`.
 *
 * @param ssv The `shared_string_view` to be hashed.
 * @return The hashed value.
 */
template<>
struct std::hash<Ghoti::shared_string_view> {
  std::size_t operator()(const Ghoti::shared_string_view & ssv) const noexcept;
};

#endif // SSV_HPP

