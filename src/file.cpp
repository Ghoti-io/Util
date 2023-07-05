/**
 * @file
 */

#include "util/file.hpp"
#include "util/errorCode.hpp"
#include <cstdlib>    // for mkstemp()
#include <filesystem> // for filesystem::
#include <iostream>
#include <sstream>
#include <unistd.h>   // for ::close()

using namespace std;
using namespace Ghoti::Util;

File::File() : path{}, isTemp{false} {}

File::File(const string & path) : path{path}, isTemp{false} {}

File::File(File && source) {
  this->path = move(source.path);
  this->isTemp = source.isTemp;
  // This is a transfer of ownership, so the source must no longer identify
  // the target file as a temp file.
  source.isTemp = false;
}

File & File::operator=(File && source) {
  swap(this->path, source.path);
  swap(this->isTemp, source.isTemp);
  return *this;
}

File::~File() {
  if (this->isTemp) {
    filesystem::remove(this->path);
  }
}

error_code File::rename(const string & destinationPath) {
  // The behavior of filesystem::rename() is implementation-specific when the
  // target already exists, and therefore unreliable for this library.
  // As such, we will check manually to verify that the destination path is not
  // already in use.  Unfortunately, this is not atomic.
  // https://en.cppreference.com/w/cpp/io/c/rename
  // https://en.cppreference.com/w/cpp/filesystem/rename
  error_code ec{};
  if (filesystem::exists(destinationPath, ec)) {
    return make_error_code(ErrorCode::FILE_EXISTS_AT_TARGET_PATH);
  }

  ec.clear();
  filesystem::rename(this->path, destinationPath, ec);
  this->isTemp = false;
  return ec;
}

error_code File::remove() {
  error_code ec{};

  if (!filesystem::remove(this->path, ec)) {
    // The file was not removed.  If there is no error, it is because there was
    // no file to be deleted in the first place.
    if (!ec) {
      ec = make_error_code(ErrorCode::FILE_DOES_NOT_EXIST);
    }
  }
  this->isTemp = false;

  return ec;
}

File File::createTemp(const std::string & pattern) {
  // Attempt to create a file in the OS temp directory.
  string path = filesystem::temp_directory_path() / (pattern + ".XXXXXX");
  char * pathCStr = const_cast<char *>(path.c_str());
  auto fd = mkstemp(pathCStr);

  // It either succeeded or it did not.
  if (fd != -1) {
    ::close(fd);
    File f{path};
    f.isTemp = true;
    return f;
  }

  return File{};
}

File::operator string() const {
  // Verify file open.
  fstream f{this->path, ios::in | ios::binary};
  if (!f.is_open()) {
    return "";
  }

  // Get the contents.
  stringstream ss;
  ss << f.rdbuf();
  f.close();
  return ss.str();
}

const string & File::getPath() const {
  return this->path;
}

error_code File::append(string_view sv) {
  // Open the file for writing then verify.
  fstream f{this->path, ios::out | ios::binary | ios::app};
  if (!f.is_open()) {
    return make_error_code(ErrorCode::FILE_COULD_NOT_BE_OPENED);
  }

  // Write then verify.
  f << sv;
  if (f.fail()) {
    return make_error_code(ErrorCode::ERROR_WRITING_TO_FILE);
  }

  return {};
}

error_code File::truncate(string_view sv) {
  // Open the file for writing then verify.
  fstream f{this->path, ios::out | ios::binary | ios::trunc};
  if (!f.is_open()) {
    return make_error_code(ErrorCode::FILE_COULD_NOT_BE_OPENED);
  }

  // Write then verify.
  f << sv;
  if (f.fail()) {
    return make_error_code(ErrorCode::ERROR_WRITING_TO_FILE);
  }

  return {};
}

error_code File::test() const noexcept {
  if (!this->path.length()) {
    return make_error_code(ErrorCode::NO_FILE_PATH_SPECIFIED);
  }
  error_code ec{};
  if (!filesystem::exists(this->path, ec)) {
    return ec
      ? ec
      : make_error_code(ErrorCode::FILE_DOES_NOT_EXIST);
  }
  return {};
}

