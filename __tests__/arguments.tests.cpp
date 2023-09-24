
// NOLINTBEGIN

#include <gtest/gtest.h>

#include <array>
#include <stdexcept>

#include "pirate.hpp"
#include "types/arg_types.hpp"

TEST(Pirate, parse) {
  pirate::Args::register_arg("config");
  pirate::Args::register_arg("a");
  pirate::Args::register_arg("b");
  pirate::Args::register_arg("c");

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

TEST(Pirate, ArgumentTypes) {
  pirate::Args::register_arg("required", pirate::ArgType::REQUIRED);
  pirate::Args::register_arg("optional", pirate::ArgType::OPTIONAL);
  pirate::Args::register_arg("req_val", pirate::ArgType::REQUIRED | pirate::ArgType::VALUE_REQUIRED);
  pirate::Args::register_arg("opt_val", pirate::ArgType::OPTIONAL | pirate::ArgType::VALUE_REQUIRED);

  int         argc1 = 5;
  const char* argv1[] = {
      "pirate", "-required", "-optional", "-req_val=test", "-opt_val",
  };
  int         argc2 = 4;
  const char* argv2[] = {
      "pirate",
      "-required",
      "-req_val",
      "-opt_val",
  };
  int         argc3 = 3;
  const char* argv3[] = {
      "pirate",
      "-req_val",
      "-opt_val",
  };
  int         argc4 = 3;
  const char* argv4[] = {
      "pirate",
      "required",
      "-req_val=test",
      "-opt_val=test",
  };
  int         argc5 = 3;
  const char* argv5[] = {
      "pirate",
      "required",
      "-req_val",
      "-opt_val=test",
  };
  int         argc6 = 4;
  const char* argv6[] = {
      "pirate",
      "-optional",
      "-req_val=test",
      "-opt_val",
  };

  pirate::Args sut1(argc1, argv1);
  EXPECT_THROW(pirate::Args sut2(argc2, argv2), std::runtime_error);
  EXPECT_THROW(pirate::Args sut3(argc3, argv3), std::runtime_error);
  pirate::Args sut4(argc4, argv4);
  EXPECT_THROW(pirate::Args sut5(argc5, argv5), std::runtime_error);
  EXPECT_THROW(pirate::Args sut6(argc6, argv6), std::runtime_error);
}

// TODO Test arg types

// NOLINTEND