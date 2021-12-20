#include <gtest/gtest.h>

#include <iostream>  // clog

#include "common.hpp"
#include "texture.hpp"

#if SDL_VERSION_ATLEAST(2, 0, 12)

TEST(ScaleMode, EnumValues)
{
  ASSERT_EQ(cen::ToUnderlying(cen::ScaleMode::Linear), SDL_ScaleModeLinear);
  ASSERT_EQ(cen::ToUnderlying(cen::ScaleMode::Nearest), SDL_ScaleModeNearest);
  ASSERT_EQ(cen::ToUnderlying(cen::ScaleMode::Best), SDL_ScaleModeBest);
}

TEST(ScaleMode, ToString)
{
  ASSERT_THROW(cen::to_string(static_cast<cen::ScaleMode>(4)), cen::Error);

  ASSERT_EQ("Nearest", cen::to_string(cen::ScaleMode::Nearest));
  ASSERT_EQ("Linear", cen::to_string(cen::ScaleMode::Linear));
  ASSERT_EQ("Best", cen::to_string(cen::ScaleMode::Best));

  std::clog << "Scale mode example: " << cen::ScaleMode::Linear << '\n';
}

#endif  // SDL_VERSION_ATLEAST(2, 0, 12)