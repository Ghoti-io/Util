/**
 * @file
 *
 * Test the general Wave server behavior.
 */

#include<string>
#include<gtest/gtest.h>
#include "shared_string_view.hpp"

using namespace std;
using namespace Ghoti;


int main(int argc, char** argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

