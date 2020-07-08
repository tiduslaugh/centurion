#ifndef CENTURION_MOUSE_BUTTON_EVENT_SOURCE
#define CENTURION_MOUSE_BUTTON_EVENT_SOURCE

#include "event.hpp"

namespace centurion::event {

CENTURION_DEF
MouseButtonEvent::MouseButtonEvent() noexcept : CommonEvent{}
{}

CENTURION_DEF
MouseButtonEvent::MouseButtonEvent(const SDL_MouseButtonEvent& event) noexcept
    : CommonEvent{event}
{}

CENTURION_DEF
void MouseButtonEvent::set_window_id(u32 id) noexcept
{
  m_event.windowID = id;
}

CENTURION_DEF
void MouseButtonEvent::set_which(u32 which) noexcept
{
  m_event.which = which;
}

CENTURION_DEF
void MouseButtonEvent::set_button(MouseButton button) noexcept
{
  m_event.button = static_cast<u8>(button);
}

CENTURION_DEF
void MouseButtonEvent::set_state(ButtonState state) noexcept
{
  m_event.state = static_cast<u8>(state);
}

CENTURION_DEF
void MouseButtonEvent::set_clicks(u8 clicks) noexcept
{
  m_event.clicks = clicks;
}

CENTURION_DEF
void MouseButtonEvent::set_x(i32 x) noexcept
{
  m_event.x = x;
}

CENTURION_DEF
void MouseButtonEvent::set_y(i32 y) noexcept
{
  m_event.y = y;
}

CENTURION_DEF
auto MouseButtonEvent::window_id() const noexcept -> u32
{
  return m_event.windowID;
}

CENTURION_DEF
auto MouseButtonEvent::which() const noexcept -> u32
{
  return m_event.which;
}

CENTURION_DEF
auto MouseButtonEvent::button() const noexcept -> MouseButton
{
  return static_cast<MouseButton>(m_event.button);
}

CENTURION_DEF
auto MouseButtonEvent::state() const noexcept -> ButtonState
{
  return static_cast<ButtonState>(m_event.state);
}

CENTURION_DEF
auto MouseButtonEvent::clicks() const noexcept -> u8
{
  return m_event.clicks;
}

CENTURION_DEF
auto MouseButtonEvent::x() const noexcept -> i32
{
  return m_event.x;
}

CENTURION_DEF
auto MouseButtonEvent::y() const noexcept -> i32
{
  return m_event.y;
}

}  // namespace centurion::event

#endif  // CENTURION_MOUSE_BUTTON_EVENT_SOURCE