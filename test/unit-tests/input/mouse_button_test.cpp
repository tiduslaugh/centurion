#include "input/mouse_button.hpp"

#include <gtest/gtest.h>

#include <iostream>  // clog

#include "core/to_underlying.hpp"

TEST(MouseButton, Values)
{
  ASSERT_EQ(SDL_BUTTON_LEFT, cen::to_underlying(cen::mouse_button::left));
  ASSERT_EQ(SDL_BUTTON_MIDDLE, cen::to_underlying(cen::mouse_button::middle));
  ASSERT_EQ(SDL_BUTTON_RIGHT, cen::to_underlying(cen::mouse_button::right));
  ASSERT_EQ(SDL_BUTTON_X1, cen::to_underlying(cen::mouse_button::x1));
  ASSERT_EQ(SDL_BUTTON_X2, cen::to_underlying(cen::mouse_button::x2));
}

TEST(MouseButton, ToString)
{
  ASSERT_THROW(cen::to_string(static_cast<cen::mouse_button>(6)), cen::cen_error);

  ASSERT_EQ("left", cen::to_string(cen::mouse_button::left));
  ASSERT_EQ("middle", cen::to_string(cen::mouse_button::middle));
  ASSERT_EQ("right", cen::to_string(cen::mouse_button::right));
  ASSERT_EQ("x1", cen::to_string(cen::mouse_button::x1));
  ASSERT_EQ("x2", cen::to_string(cen::mouse_button::x2));

  std::clog << "Mouse button example: " << cen::mouse_button::middle << '\n';
}
