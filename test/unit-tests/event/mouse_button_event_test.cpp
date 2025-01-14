#include <gtest/gtest.h>

#include "events/event.hpp"

TEST(MouseButtonEvent, Defaults)
{
  cen::mouse_button_event event;
  ASSERT_GT(event.time(), 0u);
  ASSERT_EQ(cen::event_type::mouse_button_down, event.type());
}

TEST(MouseButtonEvent, SetWindowId)
{
  cen::mouse_button_event event;

  constexpr cen::u32 id = 64;
  event.set_window_id(id);

  ASSERT_EQ(id, event.window_id());
}

TEST(MouseButtonEvent, SetWhich)
{
  cen::mouse_button_event event;

  constexpr cen::u32 which = 17;
  event.set_which(which);

  ASSERT_EQ(which, event.which());
}

TEST(MouseButtonEvent, SetButton)
{
  cen::mouse_button_event event;

  constexpr auto button = cen::mouse_button::right;
  event.set_button(button);

  ASSERT_EQ(button, event.button());
}

TEST(MouseButtonEvent, SetState)
{
  cen::mouse_button_event event;

  constexpr auto state = cen::button_state::released;
  event.set_state(state);

  ASSERT_EQ(state, event.state());
}

TEST(MouseButtonEvent, SetClicks)
{
  cen::mouse_button_event event;

  constexpr auto clicks = 2;
  event.set_clicks(clicks);

  ASSERT_EQ(clicks, event.clicks());
}

TEST(MouseButtonEvent, SetX)
{
  cen::mouse_button_event event;

  constexpr auto x = 645;
  event.set_x(x);

  ASSERT_EQ(x, event.x());
}

TEST(MouseButtonEvent, SetY)
{
  cen::mouse_button_event event;

  constexpr auto y = 177;
  event.set_y(y);

  ASSERT_EQ(y, event.y());
}

TEST(MouseButtonEvent, WindowId)
{
  SDL_MouseButtonEvent sdl;
  sdl.windowID = 75;

  const cen::mouse_button_event event{sdl};
  ASSERT_EQ(sdl.windowID, event.window_id());
}

TEST(MouseButtonEvent, Which)
{
  SDL_MouseButtonEvent sdl;
  sdl.which = 23;

  const cen::mouse_button_event event{sdl};
  ASSERT_EQ(sdl.which, event.which());
}

TEST(MouseButtonEvent, Button)
{
  SDL_MouseButtonEvent sdl;
  sdl.button = SDL_BUTTON_X1;

  const cen::mouse_button_event event{sdl};
  ASSERT_EQ(sdl.button, cen::to_underlying(event.button()));
}

TEST(MouseButtonEvent, State)
{
  SDL_MouseButtonEvent sdl;
  sdl.state = SDL_PRESSED;

  const cen::mouse_button_event event{sdl};
  ASSERT_EQ(sdl.state, cen::to_underlying(event.state()));
}

TEST(MouseButtonEvent, Pressed)
{
  SDL_MouseButtonEvent sdl;
  sdl.state = SDL_PRESSED;

  const cen::mouse_button_event event{sdl};
  ASSERT_TRUE(event.pressed());
}

TEST(MouseButtonEvent, Released)
{
  SDL_MouseButtonEvent sdl;
  sdl.state = SDL_RELEASED;

  const cen::mouse_button_event event{sdl};
  ASSERT_TRUE(event.released());
}

TEST(MouseButtonEvent, Clicks)
{
  SDL_MouseButtonEvent sdl;
  sdl.clicks = 2;

  const cen::mouse_button_event event{sdl};
  ASSERT_EQ(sdl.clicks, event.clicks());
}

TEST(MouseButtonEvent, X)
{
  SDL_MouseButtonEvent sdl;
  sdl.x = 467;

  const cen::mouse_button_event event{sdl};
  ASSERT_EQ(sdl.x, event.x());
}

TEST(MouseButtonEvent, Y)
{
  SDL_MouseButtonEvent sdl;
  sdl.y = 887;

  const cen::mouse_button_event event{sdl};
  ASSERT_EQ(sdl.y, event.y());
}

TEST(MouseButtonEvent, AsSDLEvent)
{
  const cen::mouse_button_event event;
  const auto sdl = cen::as_sdl_event(event);

  ASSERT_EQ(sdl.button.type, cen::to_underlying(event.type()));
  ASSERT_EQ(sdl.button.timestamp, event.time());
}
