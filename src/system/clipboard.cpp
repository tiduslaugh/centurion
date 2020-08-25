#include "clipboard.hpp"

#include "detail/sdl_string.hpp"

namespace centurion::clipboard {

auto has_text() noexcept -> bool
{
  return SDL_HasClipboardText();
}

auto get_text() -> std::string
{
  const detail::sdl_string text{SDL_GetClipboardText()};
  return text.copy();
}

auto set_text(nn_czstring text) noexcept -> bool
{
  return SDL_SetClipboardText(text) == 0;
}

}  // namespace centurion::clipboard