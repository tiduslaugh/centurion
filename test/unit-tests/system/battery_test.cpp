#include "system/battery.hpp"

#include <gtest/gtest.h>

TEST(Battery, Percentage)
{
  EXPECT_NO_THROW(cen::battery::percentage());

  const auto percentage = cen::battery::percentage();
  EXPECT_EQ(percentage.has_value(), cen::battery::is_available());

  if (percentage)
  {
    int actual = -1;
    SDL_GetPowerInfo(nullptr, &actual);

    EXPECT_EQ(actual, percentage);
  }
}

TEST(Battery, SecondsLeft)
{
  EXPECT_NO_THROW(cen::battery::seconds_left());

  int actual = -1;
  SDL_GetPowerInfo(&actual, nullptr);

  if (const auto secs = cen::battery::seconds_left())
  {
    EXPECT_EQ(cen::seconds<int>{actual}, secs.value());
  }
}

TEST(Battery, MinutesLeft)
{
  EXPECT_NO_THROW(cen::battery::minutes_left());

  if (const auto minutes = cen::battery::minutes_left())
  {
    int actual = -1;
    SDL_GetPowerInfo(&actual, nullptr);

    const cen::seconds<int> s{actual};
    EXPECT_EQ(std::chrono::duration_cast<cen::minutes<int>>(s),
              minutes.value());
  }
}

TEST(Battery, State)
{
  EXPECT_NO_THROW(cen::battery::state());

  const auto state = cen::battery::state();
  const auto actual = SDL_GetPowerInfo(nullptr, nullptr);

  EXPECT_EQ(actual, static_cast<SDL_PowerState>(state));
}

TEST(Battery, Exists)
{
  const auto state = cen::battery::state();
  const auto exists = state == cen::power_state::on_battery ||
                      state == cen::power_state::charged ||
                      state == cen::power_state::charging;
  EXPECT_EQ(exists, cen::battery::exists());
}

TEST(Battery, IsCharging)
{
  EXPECT_EQ(cen::battery::state() == cen::power_state::charging,
            cen::battery::is_charging());
}

TEST(Battery, IsCharged)
{
  EXPECT_EQ(cen::battery::state() == cen::power_state::charged,
            cen::battery::is_charged());
}

TEST(Battery, IsAvailable)
{
  const auto state = cen::battery::state();
  EXPECT_EQ(state != cen::power_state::unknown &&
                state != cen::power_state::no_battery,
            cen::battery::is_available());
}

TEST(Battery, PowerStateEnum)
{
  EXPECT_EQ(cen::power_state::unknown, SDL_POWERSTATE_UNKNOWN);
  EXPECT_EQ(cen::power_state::on_battery, SDL_POWERSTATE_ON_BATTERY);
  EXPECT_EQ(cen::power_state::no_battery, SDL_POWERSTATE_NO_BATTERY);
  EXPECT_EQ(cen::power_state::charging, SDL_POWERSTATE_CHARGING);
  EXPECT_EQ(cen::power_state::charged, SDL_POWERSTATE_CHARGED);

  EXPECT_EQ(SDL_POWERSTATE_UNKNOWN, cen::power_state::unknown);
  EXPECT_EQ(SDL_POWERSTATE_ON_BATTERY, cen::power_state::on_battery);
  EXPECT_EQ(SDL_POWERSTATE_NO_BATTERY, cen::power_state::no_battery);
  EXPECT_EQ(SDL_POWERSTATE_CHARGING, cen::power_state::charging);
  EXPECT_EQ(SDL_POWERSTATE_CHARGED, cen::power_state::charged);

  EXPECT_NE(cen::power_state::charged, SDL_POWERSTATE_ON_BATTERY);
  EXPECT_NE(SDL_POWERSTATE_CHARGING, cen::power_state::unknown);
}