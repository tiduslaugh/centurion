#include "detail/address_of.hpp"

#include <gtest/gtest.h>

#include <iostream>  // clog

TEST(AddressOf, Correctness)
{
  {  // Non-null pointer
    int i = 42;
    const auto str = cen::detail::address_of(&i);
    ASSERT_FALSE(str.empty());
    std::clog << "detail::address_of output: " << str << '\n';
  }

  {  // Null pointer
    int* i{};
    ASSERT_NO_THROW(cen::detail::address_of(i));

    const auto str = cen::detail::address_of(i);
    ASSERT_TRUE(str.empty());
  }
}
