/**
 * @file
 *
 * Test the general Wave server behavior.
 */

#include <string>
#include <gtest/gtest.h>
#include "util/file.hpp"
#include "util/errorCode.hpp"

using namespace std;
using namespace Ghoti::Util;

TEST(File, DefaultConstructor) {
  // Default-constructed file should not actually point to anything.
  File f{};
  EXPECT_EQ(f.getPath(), "");
  EXPECT_EQ(f.test(), make_error_code(ErrorCode::NO_FILE_PATH_SPECIFIED));
}

TEST(File, ExistingFile) {
  string path{"./build/apps/fileExists.txt"};

  // Open an existing file.
  File f{path};
  EXPECT_FALSE(f.test());
  EXPECT_EQ(f.getPath(), path);

  // Read contents.
  EXPECT_EQ(string{f}, "Hello World\n");

  // Reading should reset the file to the beginning.
  EXPECT_EQ(string{f}, "Hello World\n");
}

TEST(File, MissingFile) {
  {
    File f{"fileDoesntExist.txt"};
    EXPECT_EQ(f.test(), make_error_code(ErrorCode::FILE_DOES_NOT_EXIST));
  }
}

TEST(File, TempFile) {
  string path{};
  {
    auto f{File::createTemp("abc123")};
    EXPECT_FALSE(f.test());
    string contents{"file contents"};

    // Save the path of the temporary file so that later we can verify that the
    // file was deleted.
    path = f.getPath();

    // Write to the temporary file.
    EXPECT_FALSE(f.append(contents));

    // Read back the contents of the temporary file.
    EXPECT_EQ(string{f}, contents);

    // Write to the temporary file again.
    EXPECT_FALSE(f.append(contents));

    // Read back the contents of the temporary file.
    EXPECT_EQ(string{f}, contents + contents);

    // The temporary file will now pass out of scope and should be
    // automatically deleted.
  }

  // The file should not exist.
  File f{path};
  EXPECT_EQ(f.test(), make_error_code(ErrorCode::FILE_DOES_NOT_EXIST));
}

TEST(Delete, MissingFile) {
  // Delete a file that does not exist.
  {
    File f{"fileDoesntExist.txt"};
    EXPECT_EQ(f.test(), make_error_code(ErrorCode::FILE_DOES_NOT_EXIST));
    EXPECT_EQ(f.remove(), make_error_code(ErrorCode::FILE_DOES_NOT_EXIST));
  }
}

TEST(Delete, ExistingFile) {
  string newName{};
  string contents{"file contents"};

  // Create a temp file and rename it so that it is not automatically deleted.
  {
    auto f{File::createTemp("abc123")};
    EXPECT_FALSE(f.append(contents));

    newName = f.getPath() + ".2";
    EXPECT_FALSE(f.rename(newName));
  }

  // Open the file again to verify that the rename succeeded, then delete the
  // file.
  {
    File f{newName};
    EXPECT_EQ(string{f}, contents);

    EXPECT_FALSE(f.remove());
  }

  // Verify that the file was deleted.
  {
    File f{newName};
    EXPECT_EQ(f.test(), make_error_code(ErrorCode::FILE_DOES_NOT_EXIST));
  }
}

TEST(Rename, OverExisting) {
  string f1Path{};
  string f2Path{};
  {
    // Create two files.
    auto f1{File::createTemp("abc123")};
    auto f2{File::createTemp("abc123")};
    f1Path = f1.getPath();
    f2Path = f2.getPath();

    // Verify that they do not have the same path.
    EXPECT_NE(f1.getPath(), f2.getPath());

    // Write something into both files to confirm that they both exist.
    EXPECT_FALSE(f1.append("1"));
    EXPECT_FALSE(f2.append("2"));


    // Attempt to rename one file to that of the other.
    EXPECT_EQ(f2.rename(f1.getPath()), make_error_code(ErrorCode::FILE_EXISTS_AT_TARGET_PATH));
    EXPECT_EQ(f2.getPath(), f2Path);
  }

  // Verify that all of the rename attempts did not keep the files from being
  // properly cleaned up, since they were both temp files.
  {
    File f1{f1Path};
    File f2{f2Path};
    EXPECT_EQ(f1.test(), make_error_code(ErrorCode::FILE_DOES_NOT_EXIST));
    EXPECT_EQ(f2.test(), make_error_code(ErrorCode::FILE_DOES_NOT_EXIST));
  }
}

TEST(File, Truncate) {
  auto f{File::createTemp("abc123")};
  EXPECT_FALSE(f.append("a"));
  EXPECT_EQ(string{f}, "a");
  EXPECT_FALSE(f.truncate("b"));
  EXPECT_EQ(string{f}, "b");
}

int main(int argc, char** argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

