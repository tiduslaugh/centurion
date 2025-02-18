#include "core/result.hpp"

#include <gtest/gtest.h>

#include <iostream>  // clog

TEST(Result, Values)
{
  ASSERT_EQ(cen::result{true}, cen::success);
  ASSERT_EQ(cen::result{false}, cen::failure);

  ASSERT_NE(cen::result{false}, cen::success);
  ASSERT_NE(cen::result{true}, cen::failure);

  ASSERT_NE(cen::success, cen::failure);

  ASSERT_TRUE(cen::success);
  ASSERT_FALSE(cen::failure);
}

TEST(Result, ToString)
{
  std::clog << "result (success): \"" << cen::success << "\"\n";
  std::clog << "result (failure): \"" << cen::failure << "\"\n";
}
