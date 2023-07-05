/**
 * @file
 * Declare the Ghoti::Util::File class used to reference temporary files.
 */

#ifndef GHOTI_UTIL_FILE_HPP
#define GHOTI_UTIL_FILE_HPP

#include <string>
#include <fstream>

namespace Ghoti::Util {
/**
 * Represents a file on the file system.
 */
class File {
  public:
  /**
   * Default constructor.
   */
  File();

  /**
   * Constructor for a named path.
   *
   * @param path The file to open.
   */
  File(const std::string & path);

  /**
   * Copy constructor is deleted.
   */
  File(const File &) = delete;

  /**
   * Move constructor.
   *
   * @param source The source object.
   */
  File(File && source);

  /**
   * Copy assignment operator deleted.
   */
  File & operator=(const File &) = delete;

  /**
   * Move assignment operator.
   */
  File & operator=(File &&);

  /**
   * Destructor.  Will delete the file if it is a temp file.
   */
  ~File();

  /**
   * Move or rename the file.
   *
   * If the source file is a temp file, the destination will no longer be
   * considered to be a temp file (meaning the file will not be automatically
   * deleted when the object goes out of scope).
   *
   * If the file is open, it will be automatically closed and must be re-opened
   * explicitly.
   *
   * @param destinationPath The target name for the file.
   * @return The error code resulting from the operation (if any).
   */
  std::error_code rename(const std::string & destinationPath);

  /**
   * Remove the file.
   *
   * If the file is open, it will be automatically closed.
   *
   * @return The error code resulting from the operation (if any).
   */
  std::error_code remove();

  /**
   * Create a temporary file in the OS temp directory, matching the supplied
   * pattern.  Characters will be added to the end if the name as needed to
   * avoid file conflicts.
   *
   * @param pattern The file name pattern that should be used.
   * @return A `File` object.
   */
  static File createTemp(const std::string & pattern);

  /**
   * Return the contents of the file as a string.
   */
  operator std::string() const;

  /**
   * Get the current path.
   */
  const std::string & getPath() const;

  /**
   * Write the supplied bytes to the end of the file.
   *
   * @param sv The bytes to write to the end of the file.
   * @return The error code resulting from the operation (if any).
   */
  std::error_code append(std::string_view sv);

  /**
   * Truncate a file and write the supplied bytes to the newly trancated file.
   *
   * @param sv The bytes to write to the file.
   * @return The error code resulting from the operation (if any).
   */
  std::error_code truncate(std::string_view sv);

  /**
   * Test the file to see if it exists and return any error message that
   * results from it.
   *
   * @return The error code resulting from the operation (if any).
   */
  std::error_code test() const noexcept;

  private:
  /**
   * The path to the file.
   */
  std::string path;

  /**
   * Stores whether or not the file is a temporary file.
   */
  bool isTemp;
};
}

#endif // GHOTI_UTIL_FILE_HPP

