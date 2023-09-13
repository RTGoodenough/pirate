
// NOLINTBEGIN

#include <gtest/gtest.h>

#include <array>
#include <stdexcept>

#include "pirate.hpp"

TEST(Args, parse) {
  pirate::Args::register_arg("config", false, true);
  pirate::Args::register_arg("a", false, false);
  pirate::Args::register_arg("b", false, false);
  pirate::Args::register_arg("c", false, false);

  int         argc1 = 3;
  const char* argv1[] = {"pirate", "test", "another"};

  EXPECT_THROW(pirate::Args sut1(argc1, argv1), std::runtime_error);

  int         argc2 = 2;
  const char* argv2[] = {
      "pirate",
      "-config=test.txt",
  };

  pirate::Args sut2(argc2, argv2);
  EXPECT_EQ(sut2.count(), 1);
  EXPECT_EQ(sut2.get("config"), "test.txt");

  int         argc3 = 2;
  const char* argv3[] = {
      "pirate",
      "-abc",
  };

  pirate::Args sut3(argc3, argv3);
  EXPECT_EQ(sut3.count(), 3);
  EXPECT_EQ(sut3.get("a"), "");
  EXPECT_EQ(sut3.get("b"), "");
  EXPECT_EQ(sut3.get("c"), "");
}

// NOLINTEND