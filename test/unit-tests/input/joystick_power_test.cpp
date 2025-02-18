#include "input/joystick_power.hpp"

#include <gtest/gtest.h>

#include <iostream>  // clog

TEST(JoystickPower, Values)
{
  ASSERT_EQ(cen::joystick_power::unknown, SDL_JOYSTICK_POWER_UNKNOWN);
  ASSERT_EQ(cen::joystick_power::empty, SDL_JOYSTICK_POWER_EMPTY);
  ASSERT_EQ(cen::joystick_power::low, SDL_JOYSTICK_POWER_LOW);
  ASSERT_EQ(cen::joystick_power::medium, SDL_JOYSTICK_POWER_MEDIUM);
  ASSERT_EQ(cen::joystick_power::full, SDL_JOYSTICK_POWER_FULL);
  ASSERT_EQ(cen::joystick_power::wired, SDL_JOYSTICK_POWER_WIRED);
  ASSERT_EQ(cen::joystick_power::max, SDL_JOYSTICK_POWER_MAX);

  ASSERT_EQ(SDL_JOYSTICK_POWER_UNKNOWN, cen::joystick_power::unknown);
  ASSERT_EQ(SDL_JOYSTICK_POWER_EMPTY, cen::joystick_power::empty);
  ASSERT_EQ(SDL_JOYSTICK_POWER_LOW, cen::joystick_power::low);
  ASSERT_EQ(SDL_JOYSTICK_POWER_MEDIUM, cen::joystick_power::medium);
  ASSERT_EQ(SDL_JOYSTICK_POWER_FULL, cen::joystick_power::full);
  ASSERT_EQ(SDL_JOYSTICK_POWER_WIRED, cen::joystick_power::wired);
  ASSERT_EQ(SDL_JOYSTICK_POWER_MAX, cen::joystick_power::max);

  ASSERT_NE(cen::joystick_power::max, SDL_JOYSTICK_POWER_WIRED);
  ASSERT_NE(SDL_JOYSTICK_POWER_MEDIUM, cen::joystick_power::low);
}

TEST(JoystickPower, ToString)
{
  ASSERT_THROW(cen::to_string(static_cast<cen::joystick_power>(SDL_JOYSTICK_POWER_MAX + 1)),
               cen::cen_error);

  ASSERT_EQ("unknown", cen::to_string(cen::joystick_power::unknown));
  ASSERT_EQ("empty", cen::to_string(cen::joystick_power::empty));
  ASSERT_EQ("low", cen::to_string(cen::joystick_power::low));
  ASSERT_EQ("medium", cen::to_string(cen::joystick_power::medium));
  ASSERT_EQ("full", cen::to_string(cen::joystick_power::full));
  ASSERT_EQ("wired", cen::to_string(cen::joystick_power::wired));
  ASSERT_EQ("max", cen::to_string(cen::joystick_power::max));

  std::clog << "Joystick power example: " << cen::joystick_power::wired << '\n';
}
