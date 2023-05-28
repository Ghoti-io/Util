/**
 * @file
 *
 * Test the shared_string_view behavior.
 */

#include <map>
#include <sstream>
#include <string>
#include <gtest/gtest.h>
#include "shared_string_view.hpp"

using namespace std;
using namespace Ghoti;

TEST(Constructor, Length) {
  // SSV created with a C-string.
  {
    shared_string_view ssv{""};
    ASSERT_EQ(ssv.length(), 0);
  }
  {
    shared_string_view ssv{"a"};
    ASSERT_EQ(ssv.length(), 1);
  }
  {
    shared_string_view ssv{"abc"};
    ASSERT_EQ(ssv.length(), 3);
  }
  // SSV created with a C-string and explicit length.
  {
    shared_string_view ssv{"abc", 3};
    ASSERT_EQ(ssv.length(), 3);
  }
  {
    shared_string_view ssv{"abc", 1};
    ASSERT_EQ(ssv.length(), 1);
  }
  // SSV created with a string object.
  {
    shared_string_view ssv{string("abc")};
    ASSERT_EQ(ssv.length(), 3);
  }
  // Copy constructor
  {
    shared_string_view ssv1{"abc"};
    shared_string_view ssv2{ssv1};
    ASSERT_EQ(string_view{ssv1}, string_view{ssv2});
  }
  // Move constructor
  {
    shared_string_view ssv1{"abc"};
    shared_string_view ssv2{move(ssv1)};
    ASSERT_NE(string_view{ssv1}, string_view{ssv2});
  }
}

TEST(Operator, string_stream) {
  {
    shared_string_view ssv{"abc 123"};
    ASSERT_EQ(string_view{ssv}, "abc 123"sv);
  }
  {
    shared_string_view ssv{"abc 123"};
    ASSERT_NE(string_view{ssv}, "abc 1234"sv);
  }
  {
    shared_string_view ssv{"abc 123"};
    ASSERT_NE(string_view{ssv}, ""sv);
  }
}

TEST(Operator, Extraction) {
  {
    shared_string_view ssv{"abc 123"};
    stringstream ss;
    ss << ssv;
    ASSERT_EQ(ss.str(), "abc 123");
  }
}

TEST(Operator, ThreeWayComparison) {
  shared_string_view ssv1{"abc 123"};
  shared_string_view ssv2{"abc 123"};
  shared_string_view ssv3{"abd 123"};

  // Test various comparison operators.
  ASSERT_EQ(ssv1, ssv2);
  ASSERT_NE(ssv1, ssv3);
  ASSERT_FALSE(ssv1 < ssv2);
  ASSERT_TRUE(ssv1 <= ssv2);
  ASSERT_FALSE(ssv1 > ssv2);
  ASSERT_TRUE(ssv1 >= ssv2);
  ASSERT_FALSE(ssv2 < ssv1);
  ASSERT_TRUE(ssv1 != ssv3);
  ASSERT_FALSE(ssv1 != ssv2);
  ASSERT_TRUE(ssv3 > ssv2);
  ASSERT_NE(ssv2, ssv3);

  // Force ssv2 and ssv3 to lose their target to make sure that this won't
  // cause a segfault.
  shared_string_view ssv4{move(ssv2)};
  shared_string_view ssv5{move(ssv3)};
  ASSERT_NE(ssv4, ssv5);
  ASSERT_NE(ssv1, ssv2);
  ASSERT_FALSE(ssv1 < ssv2);
  ASSERT_TRUE(ssv1 > ssv2);
  ASSERT_EQ(ssv2, ssv3);
}

TEST(Operator, PlusEqual) {
  shared_string_view ssv{"abc 123"};
  string foo{"foo"};

  // Verify that appending to a substring (that does not reach the end of the
  // target string) does not append to the target string.
  auto sub = ssv.substr(0,3);
  sub += foo;
  ASSERT_EQ(sub, "abcfoo");
  ASSERT_EQ(sub.substr(3,3), foo);
  ASSERT_EQ(ssv, "abc 123");

  // Appending to a substring (at the end of the target string) appends to the
  // target string.
  auto numbers = ssv.substr(4,3);
  ASSERT_EQ(numbers, "123");
  numbers += "bar";
  ASSERT_EQ(numbers, "123bar");

  // Appending a shared_string_view works, too.
  ssv += ssv;
  ASSERT_EQ(ssv, "abc 123abc 123");

  // Lastly, the original shared_string_view can still be modified.
  ssv += foo;
  ASSERT_EQ(ssv, "abc 123abc 123foo");
}

TEST(Operator, Plus) {
  shared_string_view ssv{"abc"};

  ASSERT_EQ(ssv, "abc");

  // Verify simple concatenation.
  ASSERT_EQ(ssv + "123", "abc123");

  // Verify that concatenation did not alter the parent string.
  ASSERT_EQ(ssv, "abc");

  // Verify simple concatenation with other types/values.
  ASSERT_EQ(ssv + string{"123"}, "abc123");
  ASSERT_EQ(ssv + "foo", "abcfoo");

  // Concatenating shared_string_views works, too.
  ASSERT_EQ(ssv + ssv, "abcabc");

  // Verify that concatenation did not alter the parent string.
  ASSERT_EQ(ssv, "abc");
}

TEST(Method, substr) {
  shared_string_view ssv{"abcdefghijklmnopqrstuvwxyz"};

  // Standard, in bounds substrings.
  ASSERT_EQ(ssv.substr(0,1), "a"sv);
  ASSERT_EQ(ssv.substr(1,1), "b"sv);
  ASSERT_EQ(ssv.substr(0,3), "abc"sv);
  ASSERT_EQ(ssv.substr(1,3), "bcd"sv);
  ASSERT_EQ(ssv.substr(23,3), "xyz"sv);

  // Substring requested is too long, return what is available.
  ASSERT_EQ(ssv.substr(25,3), "z"sv);
  ASSERT_EQ(ssv.substr(25,3).length(), 1);

  // Substring is out of bounds.
  ASSERT_EQ(ssv.substr(26,3), ""sv);
  ASSERT_EQ(ssv.substr(26,3).length(), 0);
  ASSERT_EQ(ssv.substr(42,3), ""sv);
  ASSERT_EQ(ssv.substr(42,3).length(), 0);

  // Substring of a substring.
  ASSERT_EQ(ssv.substr(10, 10), "klmnopqrst"sv);
  ASSERT_EQ(ssv.substr(10, 10).substr(3,3), "nop"sv);

  // Verify that substring view lengths are respected.
  ASSERT_EQ(ssv.substr(10, 10).substr(3,30), "nopqrst"sv);
}

TEST(Method, ForwardIterator) {
  size_t i{0};
  for (auto ch : shared_string_view{"abcdefg"}) {
    ASSERT_EQ(ch, "abcdefg"sv[i++]);
  }
}

TEST(Method, ReverseIterator) {
  size_t i{6};
  shared_string_view ssv{"abcdefg"};

  for (auto it = ssv.rbegin(); it != ssv.rend(); ++it) {
    ASSERT_EQ(*it, "abcdefg"sv[i--]);
  }
}

TEST(Method, Index) {
  shared_string_view ssv{"abcdefghijklmnopqrstuvwxyz"};
  ASSERT_EQ(ssv[0], 'a');
  ASSERT_EQ(ssv[1], 'b');
  ASSERT_EQ(ssv[25], 'z');
}

TEST(Aux, Hash) {
  // 3 distinct string views, 2 of which have the same value.
  shared_string_view ssv1{"abc"};
  shared_string_view ssv2{shared_string_view{"123abc"}.substr(3,3)};
  shared_string_view ssv3{"123"};

  map<shared_string_view, uint32_t> m{};

  // No value should exist in the map.
  ASSERT_EQ(m.count(ssv1), 0);
  ASSERT_EQ(m.count(ssv2), 0);
  ASSERT_EQ(m.count(ssv3), 0);

  // Insert the value that is shared by `ssv1` and `ssv2`.
  // Ensure that they evaluate to be the same.
  m[ssv1] = 42;
  ASSERT_EQ(m.count(ssv1), 1);
  ASSERT_EQ(m.count(ssv2), 1);
  ASSERT_EQ(m.count(ssv3), 0);

  // Insert `ssv3`.  Ensure that it is now present in the map.
  m[ssv3] = 1;
  ASSERT_EQ(m.count(ssv3), 1);

  // Increment the value whose key is shared.
  // Verify that the shared key values consistently access the same data.
  ++m[ssv2];
  ASSERT_EQ(m[ssv1], 43);
  ASSERT_EQ(m[ssv2], 43);
  ASSERT_EQ(m[ssv3], 1);

  // Verify that the hash is the same as the std::string_view and std::string.
  ASSERT_EQ(hash<shared_string_view>{}(ssv1), hash<string_view>{}("abc"sv));
  ASSERT_EQ(hash<shared_string_view>{}(ssv1), hash<string>{}("abc"));
}

int main(int argc, char** argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

