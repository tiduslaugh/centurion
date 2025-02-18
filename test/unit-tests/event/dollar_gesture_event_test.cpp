#include <gtest/gtest.h>

#include "events/event.hpp"

TEST(DollarGestureEvent, Defaults)
{
  cen::dollar_gesture_event event;
  ASSERT_GT(event.time(), 0u);
  ASSERT_EQ(cen::event_type::dollar_gesture, event.type());
}

TEST(DollarGestureEvent, SetTouchId)
{
  cen::dollar_gesture_event event;

  constexpr auto id = 4;
  event.set_touch_id(id);

  ASSERT_EQ(id, event.touch_id());
}

TEST(DollarGestureEvent, SetGestureId)
{
  cen::dollar_gesture_event event;

  constexpr auto id = 81;
  event.set_gesture_id(id);

  ASSERT_EQ(id, event.gesture_id());
}

TEST(DollarGestureEvent, SetFingers)
{
  cen::dollar_gesture_event event;

  constexpr cen::u32 fingers = 2;
  event.set_fingers(fingers);

  ASSERT_EQ(fingers, event.finger_count());
}

TEST(DollarGestureEvent, SetError)
{
  cen::dollar_gesture_event event;

  constexpr auto error = 5.1f;
  event.set_error(error);

  ASSERT_EQ(error, event.error());
}

TEST(DollarGestureEvent, SetX)
{
  cen::dollar_gesture_event event;

  constexpr auto x = 24.8f;
  event.set_x(x);

  ASSERT_EQ(x, event.x());
}

TEST(DollarGestureEvent, SetY)
{
  cen::dollar_gesture_event event;

  constexpr auto y = -12.9f;
  event.set_y(y);

  ASSERT_EQ(y, event.y());
}

TEST(DollarGestureEvent, TouchId)
{
  SDL_DollarGestureEvent sdl;
  sdl.touchId = 9;

  const cen::dollar_gesture_event event{sdl};
  ASSERT_EQ(sdl.touchId, event.touch_id());
}

TEST(DollarGestureEvent, GestureId)
{
  SDL_DollarGestureEvent sdl;
  sdl.gestureId = 1;

  const cen::dollar_gesture_event event{sdl};
  ASSERT_EQ(sdl.gestureId, event.gesture_id());
}

TEST(DollarGestureEvent, FingerCount)
{
  SDL_DollarGestureEvent sdl;
  sdl.numFingers = 3;

  const cen::dollar_gesture_event event{sdl};
  ASSERT_EQ(sdl.numFingers, event.finger_count());
}

TEST(DollarGestureEvent, Error)
{
  SDL_DollarGestureEvent sdl;
  sdl.error = 7.4f;

  const cen::dollar_gesture_event event{sdl};
  ASSERT_EQ(sdl.error, event.error());
}

TEST(DollarGestureEvent, X)
{
  SDL_DollarGestureEvent sdl;
  sdl.x = 56.8f;

  const cen::dollar_gesture_event event{sdl};
  ASSERT_EQ(sdl.x, event.x());
}

TEST(DollarGestureEvent, Y)
{
  SDL_DollarGestureEvent sdl;
  sdl.y = 92.3f;

  const cen::dollar_gesture_event event{sdl};
  ASSERT_EQ(sdl.y, event.y());
}

TEST(DollarGestureEvent, AsSDLEvent)
{
  const cen::dollar_gesture_event event;
  const auto sdl = cen::as_sdl_event(event);

  ASSERT_EQ(sdl.dgesture.type, static_cast<cen::u32>(event.type()));
  ASSERT_EQ(sdl.dgesture.timestamp, event.time());
}
