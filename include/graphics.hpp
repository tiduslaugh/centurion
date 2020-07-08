/*
 * MIT License
 *
 * Copyright (c) 2019-2020 Albin Johansson
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

/**
 * @file graphics.hpp
 * @brief Provides the graphics related API.
 *
 * @author Albin Johansson
 * @date 2019-2020
 * @copyright MIT License
 */

#ifndef CENTURION_VIDEO_HEADER
#define CENTURION_VIDEO_HEADER

#include <SDL_image.h>
#include <SDL_render.h>
#include <SDL_video.h>

#include <type_traits>

#include "blend_mode.hpp"
#include "centurion_api.hpp"
#include "centurion_exception.hpp"
#include "centurion_fwd.hpp"
#include "centurion_utils.hpp"
#include "color.hpp"
#include "colors.hpp"
#include "error.hpp"
#include "font.hpp"
#include "point.hpp"
#include "rect.hpp"
#include "surface.hpp"
#include "window.hpp"

namespace centurion {

/**
 * @class basic_renderer
 * @brief Provides the rendering API.
 *
 * @details This class provides the general API for hardware-accelerated
 * rendering. It's recommended to not use the `basic_renderer` name directly,
 * use a custom typedef or the provided `renderer` alias.
 *
 * @par Rendering textures
 * There are quite a number of methods provided for rendering `Texture`
 * instances. There are two overload sets, `render` and `render_t`. These
 * methods can be used with either integer or floating-point accuracy. The
 * recommended general-purpose method for rendering textures is
 * @link render(const Texture&, const IRect&, const Rect<T>&) @endlink.
 *
 * @par Translation
 * Most games utilize some sort of viewport of what the player can see of the
 * game world. If you're game features some sort of movable anchor for the
 * rendering, then you need to translate the positions of the various game
 * objects when rendering. This class provides a simple API for dealing with
 * this easily. Specify the translation viewport with
 * `set_translation_viewport(const FRect&)`, and use the rendering methods
 * that feature the `_t` suffix in their names, such as `render_t`, to
 * automatically render at translated positions.
 *
 * @par Font support
 * When rendering text, it's often needed to pass around various font
 * instances. Subsequently, this class provides an API for storing shared
 * pointers to `Font` instances. The fonts are stored in an internal map, and
 * it's possible to specify what you want to use as keys for the fonts. The
 * `renderer` alias uses `std::string` for keys.
 *
 * @par Rendering text
 * There is no method for directly rendering text. Instead, use one of the
 * `text_` methods for creating a texture that contains a rendered piece
 * of text, and render that texture when needed. Naturally, you should cache
 * these textures instead of creating and destroying them in your game loop.
 *
 * @par Examples
 * Below is an example of a typical rendering method.
 * @code{.cpp}
 * #include <centurion_as_ctn.hpp>
 * #include <graphics.hpp>
 *
 * void draw(ctn::renderer& renderer)
 * {
 *   renderer.clear_with(ctn::color::black); // clear rendering target
 *
 *   // Miscellaneous rendering calls...
 *
 *   renderer.present(); // apply the rendering operations to the target
 * }
 * @endcode
 *
 * @tparam FontKey the key type used when storing associated fonts in a map.
 *
 * @since 3.0.0
 *
 * @see `SDL_Renderer`
 *
 * @headerfile graphics.hpp
 */
template <typename FontKey>
class basic_renderer final {
 public:
  /**
   * @brief Creates a renderer based on the supplied `SDL_Renderer`.
   *
   * @param renderer a pointer to the `SDL_Renderer` that will be used by the
   * renderer.
   *
   * @throws CenturionException if the supplied pointer is null.
   *
   * @since 3.0.0
   */
  explicit basic_renderer(gsl::owner<SDL_Renderer*> renderer);

  /**
   * @brief Creates a renderer based on the supplied window.
   *
   * @details By default, the internal renderer will be created using the
   * `SDL_RENDERER_ACCELERATED` and `SDL_RENDERER_PRESENTVSYNC` flags.
   *
   * @param window the associated window instance.
   * @param flags the renderer flags that will be used.
   *
   * @throws CenturionException if something goes wrong when creating the
   * Renderer.
   *
   * @since 4.0.0
   */
  explicit basic_renderer(const Window& window,
                          SDL_RendererFlags flags = defaultFlags);

  /**
   * @brief Creates a renderer by moving the supplied renderer into the new one.
   *
   * @param other the renderer that will be moved.
   */
  basic_renderer(basic_renderer&& other) noexcept;

  basic_renderer(const basic_renderer&) = delete;

  /**
   * @brief Moves the supplied renderer into this renderer.
   *
   * @param other the renderer that will be moved.
   *
   * @return the renderer that claimed the supplied renderer.
   */
  auto operator=(basic_renderer&& other) noexcept -> basic_renderer&;

  auto operator=(const basic_renderer&) -> basic_renderer& = delete;

  ~basic_renderer() noexcept;

  /**
   * @copydoc basic_renderer(gsl::owner<SDL_Renderer*>)
   */
  [[nodiscard]] static auto unique(gsl::owner<SDL_Renderer*> renderer)
      -> std::unique_ptr<basic_renderer>;

  /**
   * @copydoc basic_renderer(const Window&, SDL_RendererFlags)
   */
  [[nodiscard]] static auto unique(const Window& window,
                                   SDL_RendererFlags flags = defaultFlags)
      -> std::unique_ptr<basic_renderer>;

  /**
   * @copydoc basic_renderer(gsl::owner<SDL_Renderer*>)
   */
  [[nodiscard]] static auto shared(gsl::owner<SDL_Renderer*> renderer)
      -> std::shared_ptr<basic_renderer>;

  /**
   * @copydoc basic_renderer(const Window&, SDL_RendererFlags)
   */
  [[nodiscard]] static auto shared(const Window& window,
                                   SDL_RendererFlags flags = defaultFlags)
      -> std::shared_ptr<basic_renderer>;

  /**
   * @brief Clears the rendering target with the currently selected color.
   *
   * @since 3.0.0
   */
  void clear() noexcept;

  /**
   * @brief Clears the rendering target with the specified color.
   *
   * @note This method doesn't change the currently selected color.
   *
   * @param color the color that will be used to clear the rendering target.
   *
   * @since 5.0.0
   */
  void clear_with(const Color& color) noexcept;

  /**
   * @brief Applies the previous rendering calls to the rendering target.
   *
   * @since 3.0.0
   */
  void present() noexcept;

  /**
   * @brief Adds a font to the renderer.
   *
   * @details This method has no effect if the renderer already has a font
   * associated with the specified key or if the supplied font is null.
   *
   * @param key the key that will be associated with the font.
   * @param font the font that will be added, can safely be null.
   *
   * @since 5.0.0
   */
  void add_font(const FontKey& key, const std::shared_ptr<Font>& font);

  /**
   * @brief Removes the font associated with the specified key.
   *
   * @details This method has no effect if there is no font associated with the
   * specified key.
   *
   * @param key the key associated with the font that will be removed.
   *
   * @since 5.0.0
   */
  void remove_font(const FontKey& key);

  /**
   * @brief Renders the outline of a rectangle in the currently selected color.
   *
   * @tparam T The type of the rectangle coordinates. Must be either `int` or
   * `float`.
   *
   * @param rect the rectangle that will be rendered.
   *
   * @since 4.0.0
   */
  template <typename T>
  void draw_rect(const Rect<T>& rect) noexcept;

  /**
   * @brief Renders a filled rectangle in the currently selected color.
   *
   * @tparam T The type of the rectangle coordinates. Must be either `int` or
   * `float`.
   *
   * @param rect the rectangle that will be rendered.
   *
   * @since 4.0.0
   */
  template <typename T>
  void fill_rect(const Rect<T>& rect) noexcept;

  /**
   * @brief Renders an outlined rectangle in the currently selected color.
   *
   * @details The rendered rectangle will be translated using the current
   * translation viewport.
   *
   * @tparam T The type of the rectangle coordinates. Must be either `int` or
   * `float`.
   *
   * @param rect the rectangle that will be rendered.
   *
   * @since 4.1.0
   */
  template <typename T>
  void draw_rect_t(const Rect<T>& rect) noexcept;

  /**
   * @brief Renders a filled rectangle in the currently selected color.
   *
   * @details The rendered rectangle will be translated using the current
   * translation viewport.
   *
   * @tparam T The type of the rectangle coordinates. Must be either `int` or
   * `float`.
   *
   * @param rect the rectangle that will be rendered.
   *
   * @since 4.1.0
   */
  template <typename T>
  void fill_rect_t(const Rect<T>& rect) noexcept;

  /**
   * @brief Renders a line between the supplied points, in the currently
   * selected color.
   *
   * @tparam T The type of the point coordinates. Must be either `int` or
   * `float`.
   *
   * @param start the start point of the line.
   * @param end the end point of the line.
   *
   * @since 4.0.0
   */
  template <typename T>
  void draw_line(const Point<T>& start, const Point<T>& end) noexcept;

  /**
   * @brief Renders a collection of lines.
   *
   * @warning `Container` *must* be a collection that stores its data
   * contiguously! The behaviour of this method is undefined if this condition
   * isn't met.
   *
   * @tparam Container the container type. Must store its elements
   * contiguously, such as `std::vector` or `std::array`.
   *
   * @param container the container that holds the points that will be used
   * to render the line.
   *
   * @since 5.0.0
   */
  template <typename Container>
  void draw_lines(const Container& container) noexcept;

  /**
   * @brief Renders a texture at the specified position.
   *
   * @tparam T The type of the point coordinates. Must be either `int` or
   * `float`.
   *
   * @param texture the texture that will be rendered.
   * @param position the position of the rendered texture.
   *
   * @since 4.0.0
   */
  template <typename T>
  void render(const Texture& texture, const Point<T>& position) noexcept;

  /**
   * @brief Renders a texture according to the specified rectangle.
   *
   * @tparam T The type of the rectangle coordinates. Must be either `int` or
   * `float`.
   *
   * @param texture the texture that will be rendered.
   * @param destination the position and size of the rendered texture.
   *
   * @since 4.0.0
   */
  template <typename T>
  void render(const Texture& texture, const Rect<T>& destination) noexcept;

  /**
   * @brief Renders a texture.
   *
   * @remarks This should be your preferred method of rendering textures. This
   * method is efficient and simple.
   *
   * @tparam T The type of the rectangle coordinates. Must be either `int` or
   * `float`.
   *
   * @param texture the texture that will be rendered.
   * @param source the cutout out of the texture that will be rendered.
   * @param destination the position and size of the rendered texture.
   *
   * @since 4.0.0
   */
  template <typename T>
  void render(const Texture& texture,
              const IRect& source,
              const Rect<T>& destination) noexcept;

  /**
   * @brief Renders a texture.
   *
   * @tparam T The type of the rectangle coordinates. Must be either `int` or
   * `float`.
   *
   * @param texture the texture that will be rendered.
   * @param source the cutout out of the texture that will be rendered.
   * @param destination the position and size of the rendered texture.
   * @param angle the clockwise angle, in degrees, with which the rendered
   * texture will be rotated.
   *
   * @since 4.0.0
   */
  template <typename T>
  void render(const Texture& texture,
              const IRect& source,
              const Rect<T>& destination,
              const double angle) noexcept;

  /**
   * @brief Renders a texture.
   *
   * @tparam T The type of the rectangle coordinates. Must be either `int` or
   * `float`.
   *
   * @param texture the texture that will be rendered.
   * @param source the cutout out of the texture that will be rendered.
   * @param destination the position and size of the rendered texture.
   * @param angle the clockwise angle, in degrees, with which the rendered
   * texture will be rotated.
   * @param center specifies the point around which the rendered texture will be
   * rotated.
   *
   * @since 4.0.0
   */
  template <typename T>
  void render(const Texture& texture,
              const IRect& source,
              const Rect<T>& destination,
              const double angle,
              const Point<T>& center) noexcept;

  /**
   * @brief Renders a texture.
   *
   * @tparam T The type of the rectangle coordinates. Must be either `int` or
   * `float`.
   *
   * @param texture the texture that will be rendered.
   * @param source the cutout out of the texture that will be rendered.
   * @param destination the position and size of the rendered texture.
   * @param angle the clockwise angle, in degrees, with which the rendered
   * texture will be rotated.
   * @param center specifies the point around which the rendered texture will be
   * rotated.
   * @param flip specifies how the rendered texture will be flipped.
   *
   * @since 4.0.0
   */
  template <typename T>
  void render(const Texture& texture,
              const IRect& source,
              const Rect<T>& destination,
              const double angle,
              const Point<T>& center,
              const SDL_RendererFlip flip) noexcept;

  /**
   * @brief Renders a texture at the specified position.
   *
   * @details The rendered texture will be translated using the translation
   * viewport.
   *
   * @tparam T The type of the point coordinates. Must be either `int` or
   * `float`.
   *
   * @param texture the texture that will be rendered.
   * @param position the position (pre-translation) of the rendered texture.
   *
   * @since 4.0.0
   */
  template <typename T>
  void render_t(const Texture& texture, const Point<T>& position) noexcept;

  /**
   * @brief Renders a texture according to the specified rectangle.
   *
   * @details The rendered texture will be translated using the translation
   * viewport.
   *
   * @tparam T The type of the rectangle coordinates. Must be either `int` or
   * `float`.
   *
   * @param texture the texture that will be rendered.
   * @param destination the position (pre-translation) and size of the rendered
   * texture.
   *
   * @since 4.0.0
   */
  template <typename T>
  void render_t(const Texture& texture, const Rect<T>& destination) noexcept;

  /**
   * @brief Renders a texture.
   *
   * @details The rendered texture will be translated using the translation
   * viewport.
   *
   * @remarks This should be your preferred method of rendering textures. This
   * method is efficient and simple.
   *
   * @tparam T The type of the rectangle coordinates. Must be either `int` or
   * `float`.
   *
   * @param texture the texture that will be rendered.
   * @param source the cutout out of the texture that will be rendered.
   * @param destination the position (pre-translation) and size of the rendered
   * texture.
   *
   * @since 4.0.0
   */
  template <typename T>
  void render_t(const Texture& texture,
                const IRect& source,
                const Rect<T>& destination) noexcept;

  /**
   * @brief Renders a texture.
   *
   * @details The rendered texture will be translated using the translation
   * viewport.
   *
   * @tparam T The type of the rectangle coordinates. Must be either `int` or
   * `float`.
   *
   * @param texture the texture that will be rendered.
   * @param source the cutout out of the texture that will be rendered.
   * @param destination the position (pre-translation) and size of the rendered
   * texture.
   * @param angle the clockwise angle, in degrees, with which the rendered
   * texture will be rotated.
   *
   * @since 4.0.0
   */
  template <typename T>
  void render_t(const Texture& texture,
                const IRect& source,
                const Rect<T>& destination,
                const double angle) noexcept;

  /**
   * @brief Renders a texture.
   *
   * @details The rendered texture will be translated using the translation
   * viewport.
   *
   * @tparam T The type of the rectangle coordinates. Must be either `int` or
   * `float`.
   *
   * @param texture the texture that will be rendered.
   * @param source the cutout out of the texture that will be rendered.
   * @param destination the position (pre-translation) and size of the rendered
   * texture.
   * @param angle the clockwise angle, in degrees, with which the rendered
   * texture will be rotated.
   * @param center specifies the point around which the rendered texture will be
   * rotated.
   *
   * @since 4.0.0
   */
  template <typename T>
  void render_t(const Texture& texture,
                const IRect& source,
                const Rect<T>& destination,
                const double angle,
                const Point<T>& center) noexcept;

  /**
   * @brief Renders a texture.
   *
   * @tparam T The type of the rectangle coordinates. Must be either `int` or
   * `float`.
   *
   * @param texture the texture that will be rendered.
   * @param source the cutout out of the texture that will be rendered.
   * @param destination the position (pre-translation) and size of the rendered
   * texture.
   * @param angle the clockwise angle, in degrees, with which the rendered
   * texture will be rotated.
   * @param center specifies the point around which the rendered texture will be
   * rotated.
   * @param flip specifies how the rendered texture will be flipped.
   *
   * @since 4.0.0
   */
  template <typename T>
  void render_t(const Texture& texture,
                const IRect& source,
                const Rect<T>& destination,
                const double angle,
                const Point<T>& center,
                const SDL_RendererFlip flip) noexcept;

  /**
   * @brief Sets the color that will be used by the renderer.
   *
   * @param color the color that will be used by the renderer.
   *
   * @since 3.0.0
   */
  void set_color(const Color& color) noexcept;

  /**
   * @brief Sets the clipping area rectangle.
   *
   * @details Clipping is disabled by default.
   *
   * @param area the clip area rectangle; or `nothing` to disable clipping.
   *
   * @since 3.0.0
   */
  void set_clip(Optional<IRect> area) noexcept;

  /**
   * @brief Sets the viewport that will be used by the renderer.
   *
   * @param viewport the viewport that will be used by the renderer.
   *
   * @since 3.0.0
   */
  void set_viewport(const IRect& viewport) noexcept;

  /**
   * @brief Sets the translation viewport that will be used by the renderer.
   *
   * @details This method should be called before calling any of the `_t`
   * rendering methods, for automatic translation.
   *
   * @param viewport the rectangle that will be used as the translation
   * viewport.
   *
   * @since 3.0.0
   */
  void set_translation_viewport(const FRect& viewport) noexcept;

  /**
   * @brief Sets the blend mode that will be used by the renderer.
   *
   * @param mode the blend mode that will be used by the renderer.
   *
   * @since 3.0.0
   */
  void set_blend_mode(BlendMode mode) noexcept;

  /**
   * @brief Sets the rendering target of the renderer.
   *
   * @details The supplied texture must support being a render target.
   * Otherwise, this method will reset the render target.
   *
   * @param texture a pointer to the new target texture; `nullptr` indicates
   * that the default rendering target should be used.
   *
   * @since 3.0.0
   */
  void set_target(const Texture* texture) noexcept;

  /**
   * @brief Sets the rendering scale.
   *
   * @note This method has no effect if any of the arguments aren't
   * greater than zero.
   *
   * @param xScale the x-axis scale that will be used.
   * @param yScale the y-axis scale that will be used.
   *
   * @since 3.0.0
   */
  void set_scale(float xScale, float yScale) noexcept;

  /**
   * @brief Sets the logical size used by the renderer.
   *
   * @details This method is useful for resolution-independent rendering.
   *
   * @remarks This is also known as *virtual size* in other frameworks.
   *
   * @note This method has no effect if either of the supplied dimensions
   * aren't greater than zero.
   *
   * @param size the logical width and height that will be used.
   *
   * @since 3.0.0
   */
  void set_logical_size(const area_i& size) noexcept;

  /**
   * @brief Sets whether or not to force integer scaling for the logical
   * viewport.
   *
   * @details By default, this property is set to false. This method can be
   * useful to combat visual artefacts when doing floating-point rendering.
   *
   * @param useLogicalIntegerScale `true` if integer scaling should be used;
   * `false` otherwise.
   *
   * @since 3.0.0
   */
  void set_logical_integer_scale(bool useLogicalIntegerScale) noexcept;

  /**
   * @brief Returns the logical width that the renderer uses.
   *
   * @details By default, this property is set to 0.
   *
   * @return the logical width that the renderer uses.
   *
   * @since 3.0.0
   */
  [[nodiscard]] auto logical_width() const noexcept -> int;

  /**
   * @brief Returns the logical height that the renderer uses.
   *
   * @details By default, this property is set to 0.
   *
   * @return the logical height that the renderer uses.
   *
   * @since 3.0.0
   */
  [[nodiscard]] auto logical_height() const noexcept -> int;

  /**
   * @brief Returns the x-axis scale that the renderer uses.
   *
   * @return the x-axis scale that the renderer uses.
   *
   * @since 3.0.0
   */
  [[nodiscard]] auto x_scale() const noexcept -> float;

  /**
   * @brief Returns the y-axis scale that the renderer uses.
   *
   * @return the y-axis scale that the renderer uses.
   *
   * @since 3.0.0
   */
  [[nodiscard]] auto y_scale() const noexcept -> float;

  /**
   * @brief Returns the current clipping rectangle, if there is one active.
   *
   * @return the current clipping rectangle; or `nothing` if there is none.
   *
   * @since 3.0.0
   */
  [[nodiscard]] auto clip() const noexcept -> Optional<IRect>;

  /**
   * @brief Returns information about the renderer.
   *
   * @return information about the renderer; `nothing` if something went wrong.
   *
   * @since 3.0.0
   */
  [[nodiscard]] auto info() const noexcept -> Optional<SDL_RendererInfo>;

  /**
   * @brief Returns the output width of the renderer.
   *
   * @return the output width of the renderer.
   *
   * @since 3.0.0
   */
  [[nodiscard]] auto output_width() const noexcept -> int;

  /**
   * @brief Returns the output height of the renderer.
   *
   * @return the output height of the renderer.
   *
   * @since 3.0.0
   */
  [[nodiscard]] auto output_height() const noexcept -> int;

  /**
   * @brief Returns the output size of the renderer.
   *
   * @return the current output size of the renderer.
   *
   * @since 3.0.0
   */
  [[nodiscard]] auto output_size() const noexcept -> area_i;

  /**
   * @brief Returns the blend mode that is being used by the renderer.
   *
   * @return the blend mode that is being used.
   *
   * @since 4.0.0
   */
  [[nodiscard]] auto blend_mode() const noexcept -> BlendMode;

  /**
   * @brief Returns a bit mask of the current renderer flags.
   *
   * @note There are multiple other methods for checking if a flag is set,
   * such as `vsync_enabled` or `accelerated`, that are nicer to use than
   * this method.
   *
   * @return a bit mask of the current renderer flags.
   *
   * @see `SDL_RendererFlags`
   *
   * @since 3.0.0
   */
  [[nodiscard]] auto flags() const noexcept -> Uint32;

  /**
   * @brief Indicates whether or not the `present` method is synced with
   * the refresh rate of the screen.
   *
   * @return `true` if vsync is enabled; `false` otherwise.
   *
   * @since 3.0.0
   */
  [[nodiscard]] auto vsync_enabled() const noexcept -> bool;

  /**
   * @brief Indicates whether or not the renderer is hardware accelerated.
   *
   * @return `true` if the renderer is hardware accelerated; `false` otherwise.
   *
   * @since 3.0.0
   */
  [[nodiscard]] auto accelerated() const noexcept -> bool;

  /**
   * @brief Indicates whether or not the renderer is using software rendering.
   *
   * @return `true` if the renderer is software-based; `false` otherwise.
   *
   * @since 3.0.0
   */
  [[nodiscard]] auto software_based() const noexcept -> bool;

  /**
   * @brief Indicates whether or not the renderer supports rendering to a target
   * texture.
   *
   * @return `true` if the renderer supports target texture rendering; `false`
   * otherwise.
   *
   * @since 3.0.0
   */
  [[nodiscard]] auto supports_target_textures() const noexcept -> bool;

  /**
   * @brief Indicates whether or not the renderer uses integer scaling values
   * for logical viewports.
   *
   * @details By default, this property is set to false.
   *
   * @return `true` if the renderer uses integer scaling for logical viewports;
   * `false` otherwise.
   *
   * @since 3.0.0
   */
  [[nodiscard]] auto using_integer_logical_scaling() const noexcept -> bool;

  /**
   * @brief Indicates whether or not clipping is enabled.
   *
   * @details This is disabled by default.
   *
   * @return `true` if clipping is enabled; `false` otherwise.
   *
   * @since 3.0.0
   */
  [[nodiscard]] auto clipping_enabled() const noexcept -> bool;

  /**
   * @brief Returns the currently selected rendering color.
   *
   * @details The default color is black.
   *
   * @return the currently selected rendering color.
   *
   * @since 3.0.0
   */
  [[nodiscard]] auto color() const noexcept -> Color;

  /**
   * @brief Creates and returns a texture of blended text.
   *
   * @details Attempts to render the specified text in the supplied font using
   * the currently selected color and return the texture that contains the
   * result. Use the returned texture to actually render the text to the
   * screen. This method doesn't throw but might return null if something
   * goes wrong.
   *
   * This method renders the text at the highest quality and uses
   * anti-aliasing. Use this when you want high quality text, but beware that
   * this is the slowest alternative.
   *
   * @param text the text that will be rendered, can safely be null.
   * @param font the font that the text will be rendered in.
   *
   * @return a unique pointer to a texture that contains the rendered text;
   * `nullptr` if something went wrong.
   *
   * @since 4.0.0
   */
  [[nodiscard]] auto text_blended(gsl::czstring text,
                                  const Font& font) const noexcept
      -> std::unique_ptr<Texture>;

  /**
   * @brief Creates and returns a texture of blended and wrapped text.
   *
   * @details Attempts to render the specified text in the supplied font using
   * the currently selected color and return the texture that contains the
   * result. Use the returned texture to actually render the text to the
   * screen. This method doesn't throw but might return null if something
   * goes wrong.
   *
   * This method renders the text at the highest quality and uses
   * anti-aliasing. Use this when you want high quality text, but beware that
   * this is the slowest alternative. This method will wrap the supplied text
   * to fit the specified width. Furthermore, you can also manually control
   * the line breaks by inserting newline characters at the desired
   * breakpoints.
   *
   * @param text the text that will be rendered. You can insert newline
   * characters in the string to indicate breakpoints, can safely be null.
   * @param wrap the width in pixels which marks the point that the text will
   * be wrapped after.
   * @param font the font that the text will be rendered in.
   *
   * @return a unique pointer to a texture that contains the rendered text;
   * `nullptr` if something went wrong.
   *
   * @since 4.0.0
   */
  [[nodiscard]] auto text_blended_wrapped(gsl::czstring text,
                                          Uint32 wrap,
                                          const Font& font) const noexcept
      -> std::unique_ptr<Texture>;

  /**
   * @brief Creates and returns a texture of shaded text.
   *
   * @details Attempts to render the specified text in the supplied font using
   * the currently selected color and return the texture that contains the
   * result. Use the returned texture to actually render the text to the
   * screen. This method doesn't throw but might return null if something
   * goes wrong.
   *
   * This method renders the text using anti-aliasing and with a box
   * behind the text. This alternative is probably a bit slower than
   * rendering solid text but about as fast as blended text. Use this
   * method when you want nice text, and can live with a box around it.
   *
   * @param text the text that will be rendered, can safely be null.
   * @param bg the background color used for the box.
   * @param font the font that the text will be rendered in.
   *
   * @return a unique pointer to a texture that contains the rendered text;
   * `nullptr` if something went wrong.
   *
   * @since 4.0.0
   */
  [[nodiscard]] auto text_shaded(gsl::czstring text,
                                 const Color& bg,
                                 const Font& font) const noexcept
      -> std::unique_ptr<Texture>;

  /**
   * @brief Creates and returns a texture of solid text.
   *
   * @details Attempts to render the specified text in the supplied font using
   * the currently selected color and return the texture that contains the
   * result. Use the returned texture to actually render the text to the screen.
   * This method doesn't throw but might return null if something goes wrong.
   *
   * This method is the fastest at rendering text to a texture. It
   * doesn't use anti-aliasing so the text isn't very smooth. Use this method
   * when quality isn't as big of a concern and speed is important.
   *
   * @param text the text that will be rendered, can safely be null.
   * @param font the font that the text will be rendered in.
   *
   * @return a unique pointer to a texture that contains the rendered text;
   * `nullptr` if something went wrong.
   *
   * @since 4.0.0
   */
  [[nodiscard]] auto text_solid(gsl::czstring text,
                                const Font& font) const noexcept
      -> std::unique_ptr<Texture>;

  /**
   * @brief Returns the font associated with the specified name.
   *
   * @details This method returns null if there is no font associated with the
   * specified name.
   *
   * @param key the key associated with the desired font.
   *
   * @return the font associated with the specified name; `nullptr` if there is
   * no such font.
   *
   * @since 4.1.0
   */
  [[nodiscard]] auto font(const FontKey& key) noexcept -> std::shared_ptr<Font>;

  /**
   * @brief Indicates whether or not the renderer has a font associated with the
   * specified key.
   *
   * @param key the key that will be checked.
   *
   * @return `true` if the renderer has a font associated with the key; `false`
   * otherwise.
   *
   * @since 4.1.0
   */
  [[nodiscard]] auto has_font(const FontKey& key) const noexcept -> bool;

  /**
   * @brief Returns the viewport that the renderer uses.
   *
   * @return the viewport that the renderer uses.
   *
   * @since 3.0.0
   */
  [[nodiscard]] auto viewport() const noexcept -> IRect;

  /**
   * @brief Returns a textual representation of the renderer.
   *
   * @return a textual representation of the renderer.
   *
   * @since 3.0.0
   */
  [[nodiscard]] auto to_string() const -> std::string;

  /**
   * @brief Returns the translation viewport that is currently being used.
   *
   * @details Set to (0, 0, 0, 0) by default.
   *
   * @return the translation viewport that is currently being used.
   *
   * @since 3.0.0
   */
  [[nodiscard]] auto translation_viewport() const noexcept -> const FRect&;

  /**
   * @brief Returns a pointer to the associated SDL_Renderer.
   *
   * @warning Use of this method is not recommended, since it purposefully
   * breaks const-correctness. However, it's useful since many SDL calls use
   * non-const pointers even when no change will be applied.
   *
   * @return a pointer to the associated SDL_Renderer.
   *
   * @since 4.0.0
   */
  [[nodiscard]] auto get() const noexcept -> SDL_Renderer*;

  /**
   * @brief Converts to `SDL_Renderer*`.
   *
   * @return a pointer to the associated `SDL_Renderer` instance.
   *
   * @since 3.0.0
   */
  [[nodiscard]] explicit operator SDL_Renderer*() noexcept;

  /**
   * @brief Converts to `const SDL_Renderer*`.
   *
   * @return a pointer to the associated `SDL_Renderer` instance.
   *
   * @since 3.0.0
   */
  [[nodiscard]] explicit operator const SDL_Renderer*() const noexcept;

  /**
   * @brief Returns the number of available rendering drivers.
   *
   * @note Usually there is only one available rendering driver.
   *
   * @return the number of available rendering drivers.
   *
   * @since 4.0.0
   */
  [[nodiscard]] static auto render_drivers() noexcept -> int;

  /**
   * @brief Returns the number of available video drivers compiled into SDL.
   *
   * @return the number of available video drivers compiled into SDL.
   *
   * @since 4.0.0
   */
  [[nodiscard]] static auto video_drivers() noexcept -> int;

  /**
   * @brief Returns the information associated with a rendering driver.
   *
   * @param index the index of the rendering driver to query.
   *
   * @return information about the specified rendering driver; `nothing` if
   * something went wrong.
   *
   * @since 4.0.0
   */
  [[nodiscard]] static auto driver_info(int index) noexcept
      -> Optional<SDL_RendererInfo>;

 private:
  SDL_Renderer* m_renderer = nullptr;
  FRect m_translationViewport;
  std::unordered_map<FontKey, std::shared_ptr<Font>> m_fonts;

  static constexpr SDL_RendererFlags defaultFlags =
      static_cast<SDL_RendererFlags>(SDL_RENDERER_ACCELERATED |
                                     SDL_RENDERER_PRESENTVSYNC);

  /**
   * @brief Destroys the resources associated with the renderer.
   *
   * @since 4.0.0
   */
  void destroy() noexcept
  {
    if (m_renderer) {
      SDL_DestroyRenderer(m_renderer);
    }
  }

  /**
   * @brief Moves the contents of the supplied renderer instance into this
   * instance.
   *
   * @param other the instance that will be moved.
   *
   * @since 4.0.0
   */
  void move(basic_renderer&& other) noexcept
  {
    destroy();

    m_renderer = other.m_renderer;
    m_translationViewport = other.m_translationViewport;

    other.m_renderer = nullptr;
  }

  /**
   * @brief A helper method used by text rendering methods to create surfaces
   * based on the text and then convert it to fast textures.
   *
   * @param text the text that will be rendered.
   * @param render a lambda with `void(SDL_Surface*, gsl::czstring)` as its
   * signature.
   *
   * @return a unique pointer to a texture; `nullptr` if something went wrong.
   *
   * @since 4.0.0
   */
  template <typename Lambda>
  [[nodiscard]] auto render_text(gsl::czstring text,
                                 Lambda&& render) const noexcept
      -> std::unique_ptr<Texture>
  {
    if (!text) {
      return nullptr;
    }

    SDL_Surface* surface = render(text);
    if (!surface) {
      return nullptr;
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(m_renderer, surface);
    SDL_FreeSurface(surface);

    if (texture) {
      return std::make_unique<Texture>(texture);
    } else {
      return nullptr;
    }
  }

  /**
   * @brief Returns the translated x-coordinate that corresponds to the supplied
   * x-coordinate.
   *
   * @param x the x-coordinate that will be translated.
   *
   * @return the translated x-coordinate that corresponds to the supplied
   * x-coordinate.
   *
   * @since 4.1.0
   */
  template <typename T>
  [[nodiscard]] auto tx(T x) const noexcept -> T
  {
    if constexpr (std::is_same_v<T, int>) {
      return x - static_cast<T>(m_translationViewport.x());
    } else {
      return x - m_translationViewport.x();
    }
  }

  /**
   * @brief Returns the translated y-coordinate that corresponds to the supplied
   * y-coordinate.
   *
   * @param y the y-coordinate that will be translated.
   *
   * @return the translated y-coordinate that corresponds to the supplied
   * y-coordinate.
   *
   * @since 4.1.0
   */
  template <typename T>
  [[nodiscard]] auto ty(T y) const noexcept -> T
  {
    if constexpr (std::is_same_v<T, int>) {
      return y - static_cast<T>(m_translationViewport.y());
    } else {
      return y - m_translationViewport.y();
    }
  }

  template <typename T>
  [[nodiscard]] auto translate(const Point<T>& point) const noexcept -> Point<T>
  {
    return Point<T>{tx(point.x()), tx(point.y())};
  }

  template <typename T>
  [[nodiscard]] auto translate(const Rect<T>& rect) const noexcept -> Rect<T>
  {
    return {translate(rect.position()), rect.size()};
  }
};

/**
 * @typedef renderer
 * @brief Alias for a renderer that uses `std::string` as keys for fonts.
 *
 * @since 5.0.0
 *
 * @headerfile graphics.hpp
 */
using renderer = basic_renderer<std::string>;

/**
 * @typedef renderer_i
 * @brief Alias for a renderer that uses `int` as keys for fonts.
 *
 * @since 5.0.0
 *
 * @headerfile graphics.hpp
 */
using renderer_i = basic_renderer<int>;

/**
 * @class Texture
 *
 * @brief Represents an hardware-accelerated texture.
 *
 * @since 3.0.0
 *
 * @see `SDL_Texture`
 *
 * @headerfile graphics.hpp
 */
class Texture final {
 public:
  /**
   * @enum Access
   *
   * @brief Mirrors the `SDL_TextureAccess` enum.
   *
   * @since 3.0.0
   *
   * @see `SDL_TextureAccess`
   *
   * @headerfile graphics.hpp
   */
  enum class Access {
    Static = SDL_TEXTUREACCESS_STATIC, /**< Indicates that the texture changes
                                          rarely, and isn't lockable. */
    Streaming =
        SDL_TEXTUREACCESS_STREAMING, /**< Indicates that the texture
                                      * changes frequently, and is lockable. */

    Target = SDL_TEXTUREACCESS_TARGET /**< Indicates that the texture can be
                                       * used as a render target. */
  };

  /**
   * @enum ScaleMode
   *
   * @brief Mirrors the `SDL_ScaleMode` enum.
   *
   * @since 4.0.0
   *
   * @see `SDL_ScaleMode`
   *
   * @headerfile graphics.hpp
   */
  enum class ScaleMode {
    Nearest = SDL_ScaleModeNearest, /**< Represents nearest pixel sampling. */
    Linear = SDL_ScaleModeLinear,   /**< Represents linear filtering. */
    Best = SDL_ScaleModeBest        /**< Represents anisotropic filtering. */
  };

  /**
   * @brief Creates an texture from a pre-existing SDL texture.
   *
   * @note The created texture will claim ownership of the supplied pointer.
   *
   * @param texture a pointer to the SDL_Texture that will be claimed, may not
   * be null.
   *
   * @throws CenturionException if the supplied pointer is null.
   *
   * @since 3.0.0
   */
  CENTURION_API
  explicit Texture(gsl::owner<SDL_Texture*> texture);

  /**
   * @brief Creates a texture based the image at the specified path.
   *
   * @param renderer the renderer that will be used to create the texture.
   * @param path the file path of the texture, may not be null.
   *
   * @throws CenturionException if the texture cannot be loaded.
   *
   * @since 4.0.0
   */
  template <typename T>
  Texture(const basic_renderer<T>& renderer, gsl::czstring path)
  {
    if (!path) {
      throw CenturionException{"Can't load texture from null path!"};
    }

    m_texture = IMG_LoadTexture(renderer.get(), path);
    if (!m_texture) {
      throw detail::Error::from_image("Failed to create Texture!");
    }
  }

  /**
   * @brief Creates an texture that is a copy of the supplied surface.
   *
   * @param renderer the renderer that will be used to create the texture.
   * @param surface the surface that the texture will be based on.
   *
   * @throws CenturionException if the texture cannot be loaded.
   *
   * @since 4.0.0
   */
  template <typename T>
  Texture(const basic_renderer<T>& renderer, const Surface& surface)
  {
    this->m_texture =
        SDL_CreateTextureFromSurface(renderer.get(), surface.get());
    if (!m_texture) {
      throw detail::Error::from_core("Failed to create Texture from Surface!");
    }
  }

  /**
   * @brief Creates an texture with the specified characteristics.
   *
   * @param renderer the associated renderer instance.
   * @param format the pixel format of the created texture.
   * @param access the access of the created texture.
   * @param size the size of the texture.
   *
   * @throws CenturionException if the texture cannot be created.
   *
   * @since 4.0.0
   */
  template <typename T>
  Texture(const basic_renderer<T>& renderer,
          PixelFormat format,
          Access access,
          area_i size)
  {
    m_texture = SDL_CreateTexture(renderer.get(),
                                  static_cast<Uint32>(format),
                                  static_cast<int>(access),
                                  size.width,
                                  size.height);
    if (!m_texture) {
      throw detail::Error::from_core("Failed to create Texture!");
    }
  }

  /**
   * @brief Creates an texture by moving the supplied texture.
   *
   * @param other the texture that will be moved.
   *
   * @since 3.0.0
   */
  CENTURION_API
  Texture(Texture&& other) noexcept;

  Texture(const Texture&) = delete;

  auto operator=(const Texture&) -> Texture& = delete;

  /**
   * @brief Moves the supplied texture into this texture.
   *
   * @param other the texture that will be moved.
   *
   * @return the changed texture.
   *
   * @since 3.0.0
   */
  CENTURION_API
  auto operator=(Texture&& other) noexcept -> Texture&;

  CENTURION_API
  ~Texture() noexcept;

  /**
   * @copydoc Texture(gsl::owner<SDL_Texture*>)
   */
  CENTURION_QUERY
  static auto unique(gsl::owner<SDL_Texture*> texture)
      -> std::unique_ptr<Texture>;

  /**
   * @copydoc Texture(const basic_renderer<T>&, gsl::czstring)
   */
  template <typename T>
  [[nodiscard]] static auto unique(const basic_renderer<T>& renderer,
                                   gsl::czstring path)
      -> std::unique_ptr<Texture>
  {
    return std::make_unique<Texture>(renderer, path);
  }

  /**
   * @copydoc Texture(const basic_renderer<T>&, const Surface&)
   */
  template <typename T>
  [[nodiscard]] static auto unique(const basic_renderer<T>& renderer,
                                   const Surface& surface)
      -> std::unique_ptr<Texture>
  {
    return std::make_unique<Texture>(renderer, surface);
  }

  /**
   * @copydoc Texture(const basic_renderer<T>&, PixelFormat, Access, area_i)
   */
  template <typename T>
  [[nodiscard]] static auto unique(const basic_renderer<T>& renderer,
                                   PixelFormat format,
                                   Access access,
                                   area_i size) -> std::unique_ptr<Texture>
  {
    return std::make_unique<Texture>(renderer, format, access, size);
  }

  /**
   * @copydoc Texture(gsl::owner<SDL_Texture*>)
   */
  CENTURION_QUERY
  static auto shared(gsl::owner<SDL_Texture*> texture)
      -> std::shared_ptr<Texture>;

  /**
   * @copydoc Texture(const basic_renderer<T>&, gsl::czstring)
   */
  template <typename T>
  [[nodiscard]] static auto shared(const basic_renderer<T>& renderer,
                                   gsl::czstring path)
      -> std::shared_ptr<Texture>
  {
    return std::make_shared<Texture>(renderer, path);
  }

  /**
   * @copydoc Texture(const basic_renderer<T>&, const Surface&)
   */
  template <typename T>
  [[nodiscard]] static auto shared(const basic_renderer<T>& renderer,
                                   const Surface& surface)
      -> std::shared_ptr<Texture>
  {
    return std::make_shared<Texture>(renderer, surface);
  }

  /**
   * @copydoc Texture(const basic_renderer<T>&, PixelFormat, Access, area_i)
   */
  template <typename T>
  [[nodiscard]] static auto shared(const basic_renderer<T>& renderer,
                                   PixelFormat format,
                                   Access access,
                                   area_i size) -> std::shared_ptr<Texture>
  {
    return std::make_shared<Texture>(renderer, format, access, size);
  }

  /**
   * @brief Creates and returns a unique pointer to a texture.
   *
   * @details The create texture is based on the image at the specified path
   * with the `Streaming` texture access.
   *
   * @param renderer the renderer that will be used to create the texture.
   * @param path the path of the image file to base the texture on.
   * @param format the pixel format that will be used by the texture.
   *
   * @throws CenturionException if something goes wrong.
   *
   * @return a unique pointer to a texture with `Streaming` texture access.
   *
   * @since 4.0.0
   */
  template <typename T>
  [[nodiscard]] static auto streaming(const basic_renderer<T>& renderer,
                                      gsl::czstring path,
                                      PixelFormat format)
      -> std::unique_ptr<Texture>
  {
    const auto blendMode = BlendMode::Blend;
    const auto createSurface = [blendMode](gsl::czstring path,
                                           PixelFormat format) {
      Surface source{path};
      source.set_blend_mode(blendMode);
      return source.convert(format);
    };
    const auto surface = createSurface(path, format);
    auto texture = Texture::unique(renderer,
                                   format,
                                   Access::Streaming,
                                   {surface.width(), surface.height()});
    texture->set_blend_mode(blendMode);

    Uint32* pixels = nullptr;
    const auto success = texture->lock(&pixels);
    if (!success) {
      throw CenturionException{"Failed to lock texture!"};
    }

    const auto maxCount =
        static_cast<std::size_t>(surface.pitch() * surface.height());
    memcpy(pixels, surface.pixels(), maxCount);

    texture->unlock();

    return texture;
  }

  /**
   * @brief Sets the color of the pixel at the specified coordinate.
   *
   * @details This method has no effect if the texture access isn't
   * `Streaming` or if the coordinate is out-of-bounds.
   *
   * @param pixel the pixel that will be changed.
   * @param color the new color of the pixel.
   *
   * @since 4.0.0
   */
  CENTURION_API
  void set_pixel(IPoint pixel, const Color& color) noexcept;

  /**
   * @brief Sets the alpha value of the texture.
   *
   * @param alpha the alpha value, in the range [0, 255].
   *
   * @since 3.0.0
   */
  CENTURION_API
  void set_alpha(Uint8 alpha) noexcept;

  /**
   * @brief Sets the blend mode that will be used by the texture.
   *
   * @param mode the blend mode that will be used.
   *
   * @since 3.0.0
   */
  CENTURION_API
  void set_blend_mode(BlendMode mode) noexcept;

  /**
   * @brief Sets the color modulation of the texture.
   *
   * @note The alpha component in the color struct is ignored by this method.
   *
   * @param color the color that will be used to modulate the color of the
   * texture.
   *
   * @since 3.0.0
   */
  CENTURION_API
  void set_color_mod(Color color) noexcept;

  /**
   * @brief Sets the scale mode that will be used by the texture.
   *
   * @param mode the scale mode that will be used.
   *
   * @since 4.0.0
   */
  CENTURION_API
  void set_scale_mode(ScaleMode mode) noexcept;

  /**
   * @brief Returns the pixel format that is used by the texture.
   *
   * @return the pixel format that is used by the texture.
   *
   * @since 3.0.0
   */
  CENTURION_QUERY
  auto format() const noexcept -> PixelFormat;

  /**
   * @brief Returns the texture access of the texture.
   *
   * @return the texture access of the texture.
   *
   * @since 3.0.0
   */
  CENTURION_QUERY
  auto access() const noexcept -> Access;

  /**
   * @brief Returns the width of the texture.
   *
   * @return the width of the texture.
   *
   * @since 3.0.0
   */
  CENTURION_QUERY
  auto width() const noexcept -> int;

  /**
   * @brief Returns the height of the texture.
   *
   * @return the height of the texture.
   *
   * @since 3.0.0
   */
  CENTURION_QUERY
  auto height() const noexcept -> int;

  /**
   * @brief Returns the size of the texture.
   *
   * @return the size of the texture.
   *
   * @since 4.0.0
   */
  CENTURION_QUERY
  auto size() const noexcept -> area_i;

  /**
   * @brief Indicates whether or not the texture is a possible render target.
   *
   * @return `true` if the texture is a possible render target; `false`
   * otherwise.
   *
   * @since 3.0.0
   */
  CENTURION_QUERY
  auto is_target() const noexcept -> bool;

  /**
   * @brief Indicates whether or not the texture has static texture access.
   *
   * @return `true` if the texture has static texture access.
   *
   * @since 3.0.0
   */
  CENTURION_QUERY
  auto is_static() const noexcept -> bool;

  /**
   * @brief Indicates whether or not the texture has streaming texture access.
   *
   * @return `true` if the texture has streaming texture access; `false`
   * otherwise.
   *
   * @since 3.0.0
   */
  CENTURION_QUERY
  auto is_streaming() const noexcept -> bool;

  /**
   * @brief Returns the alpha value of the texture.
   *
   * @return the alpha value of the texture.
   *
   * @since 3.0.0
   */
  CENTURION_QUERY
  auto alpha() const noexcept -> Uint8;

  /**
   * @brief Returns the blend mode of the texture.
   *
   * @return the blend mode of the texture.
   *
   * @since 3.0.0
   */
  CENTURION_QUERY
  auto blend_mode() const noexcept -> BlendMode;

  /**
   * @brief Returns the color modulation of the texture.
   *
   * @return the modulation of the texture.
   *
   * @since 3.0.0
   */
  CENTURION_QUERY
  auto color_mod() const noexcept -> Color;

  /**
   * @brief Returns the scale mode that is used by the texture.
   *
   * @return the scale mode that is used by the texture.
   *
   * @since 4.0.0
   */
  CENTURION_QUERY
  auto scale_mode() const noexcept -> ScaleMode;

  /**
   * @brief Returns a string representation of the texture.
   *
   * @return a string representation of the texture.
   *
   * @since 3.0.0
   */
  CENTURION_QUERY
  auto to_string() const -> std::string;

  /**
   * @brief Returns a pointer to the associated `SDL_Texture`.
   *
   * @warning Use of this method is not recommended, since it purposefully
   * breaks const-correctness. However it's useful since many SDL calls use
   * non-const pointers even when no change will be applied.
   *
   * @return a pointer to the associated `SDL_Texture`.
   *
   * @since 4.0.0
   */
  [[nodiscard]] auto get() const noexcept -> SDL_Texture* { return m_texture; }

  /**
   * @brief Converts to `SDL_Texture*`.
   *
   * @return a pointer to the associated `SDL_Texture`.
   *
   * @since 3.0.0
   */
  [[nodiscard]] explicit operator SDL_Texture*() noexcept { return m_texture; }

  /**
   * @brief Converts to `const SDL_Texture*`.
   *
   * @return a pointer to the associated `SDL_Texture`.
   *
   * @since 3.0.0
   */
  [[nodiscard]] explicit operator const SDL_Texture*() const noexcept
  {
    return m_texture;
  }

 private:
  SDL_Texture* m_texture = nullptr;

  /**
   * @brief Destroys the internal texture instance.
   *
   * @details This method has no effect if the associated texture is already
   * null.
   *
   * @since 4.0.0
   */
  void destroy() noexcept;

  /**
   * @brief Moves the contents of the supplied texture instance into this
   * instance.
   *
   * @param other the instance that will be moved.
   *
   * @since 4.0.0
   */
  void move(Texture&& other) noexcept;

  /**
   * @brief Locks the texture for write-only pixel access.
   *
   * @remarks This method is only applicable if the texture access of the
   * texture is `Streaming`.
   *
   * @param pixels this will be filled with a pointer to the locked pixels.
   * @param pitch This is filled in with the pitch of the locked pixels, can
   * safely be null if it isn't needed.
   *
   * @return `true` if all went well; `false` otherwise.
   *
   * @since 4.0.0
   */
  CENTURION_API
  auto lock(Uint32** pixels, int* pitch = nullptr) noexcept -> bool;

  /**
   * @brief Unlocks the texture.
   *
   * @since 4.0.0
   */
  CENTURION_API
  void unlock() noexcept;
};

static_assert(std::is_final_v<Texture>);
static_assert(std::is_nothrow_move_constructible_v<Texture>);
static_assert(std::is_nothrow_move_assignable_v<Texture>);
static_assert(!std::is_nothrow_copy_constructible_v<Texture>);
static_assert(!std::is_nothrow_copy_assignable_v<Texture>);

/**
 * @brief Indicates whether or not the two texture access values are the same.
 *
 * @param lhs the lhs texture access value.
 * @param rhs the rhs texture access value.
 *
 * @return `true` if the texture access values are the same; `false` otherwise.
 *
 * @since 3.0.0
 */
[[nodiscard]] inline constexpr auto operator==(Texture::Access lhs,
                                               SDL_TextureAccess rhs) noexcept
    -> bool
{
  return static_cast<SDL_TextureAccess>(lhs) == rhs;
}

/**
 * @copydoc operator==(Texture::Access, SDL_TextureAccess)
 */
[[nodiscard]] inline constexpr auto operator==(SDL_TextureAccess lhs,
                                               Texture::Access rhs) noexcept
    -> bool
{
  return rhs == lhs;
}

/**
 * @brief Indicates whether or not the two texture access values aren't the
 * same.
 *
 * @param lhs the lhs texture access value.
 * @param rhs the rhs texture access value.
 *
 * @return `true` if the texture access values aren't the same; `false`
 * otherwise.
 *
 * @since 3.0.0
 */
[[nodiscard]] inline constexpr auto operator!=(Texture::Access lhs,
                                               SDL_TextureAccess rhs) noexcept
    -> bool
{
  return !(lhs == rhs);
}

/**
 * @copydoc operator!=(Texture::Access, SDL_TextureAccess)
 */
[[nodiscard]] inline constexpr auto operator!=(SDL_TextureAccess lhs,
                                               Texture::Access rhs) noexcept
    -> bool
{
  return !(lhs == rhs);
}

/**
 * @brief Indicates whether or not the two scale mode values are the same.
 *
 * @param lhs the lhs scale mode value.
 * @param rhs the rhs scale mode value.
 *
 * @return `true` if the scale mode values are the same; `false` otherwise.
 *
 * @since 4.0.0
 */
[[nodiscard]] inline constexpr auto operator==(Texture::ScaleMode lhs,
                                               SDL_ScaleMode rhs) noexcept
    -> bool
{
  return static_cast<SDL_ScaleMode>(lhs) == rhs;
}

/**
 * @copydoc operator==(Texture::ScaleMode, SDL_ScaleMode)
 */
[[nodiscard]] inline constexpr auto operator==(SDL_ScaleMode lhs,
                                               Texture::ScaleMode rhs) noexcept
    -> bool
{
  return rhs == lhs;
}

/**
 * @brief Indicates whether or not the two scale mode values aren't the same.
 *
 * @param lhs the lhs scale mode value.
 * @param rhs the rhs scale mode value.
 *
 * @return `true` if the scale mode values aren't the same; `false` otherwise.
 *
 * @since 4.0.0
 */
[[nodiscard]] inline constexpr auto operator!=(Texture::ScaleMode lhs,
                                               SDL_ScaleMode rhs) noexcept
    -> bool
{
  return !(lhs == rhs);
}

/**
 * @copydoc operator!=(Texture::ScaleMode, SDL_ScaleMode)
 */
[[nodiscard]] inline constexpr auto operator!=(SDL_ScaleMode lhs,
                                               Texture::ScaleMode rhs) noexcept
    -> bool
{
  return !(lhs == rhs);
}

template <typename FontKey>
basic_renderer<FontKey>::basic_renderer(gsl::owner<SDL_Renderer*> renderer)
{
  if (!renderer) {
    throw CenturionException{"Can't create renderer from null SDL_Renderer!"};
  }
  this->m_renderer = renderer;

  set_color(color::black);
  set_logical_integer_scale(false);
}

template <typename FontKey>
basic_renderer<FontKey>::basic_renderer(const Window& window,
                                        SDL_RendererFlags flags)
{
  m_renderer = SDL_CreateRenderer(window.get(), -1, flags);
  if (!m_renderer) {
    throw detail::Error::from_core("Failed to create Renderer!");
  }

  set_blend_mode(BlendMode::Blend);
  set_color(color::black);
  set_logical_integer_scale(false);
}

template <typename FontKey>
basic_renderer<FontKey>::basic_renderer(basic_renderer&& other) noexcept
{
  move(std::move(other));
}

template <typename FontKey>
auto basic_renderer<FontKey>::operator=(basic_renderer&& other) noexcept
    -> basic_renderer&
{
  if (this != &other) {
    move(std::move(other));
  }
  return *this;
}

template <typename FontKey>
basic_renderer<FontKey>::~basic_renderer() noexcept
{
  destroy();
}

template <typename FontKey>
auto basic_renderer<FontKey>::unique(gsl::owner<SDL_Renderer*> renderer)
    -> std::unique_ptr<basic_renderer>
{
  return std::make_unique<basic_renderer<FontKey>>(renderer);
}

template <typename FontKey>
auto basic_renderer<FontKey>::unique(const Window& window,
                                     SDL_RendererFlags flags)
    -> std::unique_ptr<basic_renderer>
{
  return std::make_unique<basic_renderer<FontKey>>(window, flags);
}

template <typename FontKey>
auto basic_renderer<FontKey>::shared(gsl::owner<SDL_Renderer*> renderer)
    -> std::shared_ptr<basic_renderer>
{
  return std::make_shared<basic_renderer<FontKey>>(renderer);
}

template <typename FontKey>
auto basic_renderer<FontKey>::shared(const Window& window,
                                     SDL_RendererFlags flags)
    -> std::shared_ptr<basic_renderer>
{
  return std::make_shared<basic_renderer<FontKey>>(window, flags);
}

template <typename FontKey>
void basic_renderer<FontKey>::clear() noexcept
{
  SDL_RenderClear(m_renderer);
}

template <typename FontKey>
void basic_renderer<FontKey>::clear_with(const Color& c) noexcept
{
  const auto oldColor = color();

  set_color(c);
  clear();

  set_color(oldColor);
}

template <typename FontKey>
void basic_renderer<FontKey>::present() noexcept
{
  SDL_RenderPresent(m_renderer);
}

template <typename FontKey>
void basic_renderer<FontKey>::add_font(const FontKey& key,
                                       const std::shared_ptr<Font>& font)
{
  if (!m_fonts.count(key)) {
    m_fonts.emplace(key, font);
  }
}

template <typename FontKey>
void basic_renderer<FontKey>::remove_font(const FontKey& key)
{
  m_fonts.erase(key);
}

template <typename FontKey>
template <typename T>
void basic_renderer<FontKey>::draw_rect(const Rect<T>& rect) noexcept
{
  static_assert(std::is_same_v<T, float> || std::is_same_v<T, int>);

  if constexpr (std::is_same_v<T, int>) {
    SDL_RenderDrawRect(m_renderer, static_cast<const SDL_Rect*>(rect));
  } else {
    SDL_RenderDrawRectF(m_renderer, static_cast<const SDL_FRect*>(rect));
  }
}

template <typename FontKey>
template <typename T>
void basic_renderer<FontKey>::fill_rect(const Rect<T>& rect) noexcept
{
  static_assert(std::is_same_v<T, float> || std::is_same_v<T, int>);

  if constexpr (std::is_same_v<T, int>) {
    SDL_RenderFillRect(m_renderer, static_cast<const SDL_Rect*>(rect));
  } else {
    SDL_RenderFillRectF(m_renderer, static_cast<const SDL_FRect*>(rect));
  }
}

template <typename FontKey>
template <typename T>
void basic_renderer<FontKey>::draw_rect_t(const Rect<T>& rect) noexcept
{
  draw_rect(translate(rect));
}

template <typename FontKey>
template <typename T>
void basic_renderer<FontKey>::fill_rect_t(const Rect<T>& rect) noexcept
{
  fill_rect(translate(rect));
}

template <typename FontKey>
template <typename T>
void basic_renderer<FontKey>::draw_line(const Point<T>& start,
                                        const Point<T>& end) noexcept
{
  static_assert(std::is_same_v<T, float> || std::is_same_v<T, int>);

  if constexpr (std::is_same_v<T, int>) {
    SDL_RenderDrawLine(m_renderer, start.x(), start.y(), end.x(), end.y());
  } else {
    SDL_RenderDrawLineF(m_renderer, start.x(), start.y(), end.x(), end.y());
  }
}

template <typename FontKey>
template <typename Container>
void basic_renderer<FontKey>::draw_lines(const Container& container) noexcept
{
  // This must be a point of int or float
  using point_t = typename Container::value_type;  // TODO doc

  static_assert(std::is_same_v<point_t, Point<float>> ||
                std::is_same_v<point_t, Point<int>>);

  // This is either int or float
  using value_t = typename point_t::value_type;

  if (!container.empty()) {
    const Point<value_t>& front = container.front();

    if constexpr (std::is_same_v<value_t, int>) {
      const auto* first = static_cast<const SDL_Point*>(front);
      SDL_RenderDrawLines(m_renderer, first, container.size());
    } else {
      const auto* first = static_cast<const SDL_FPoint*>(front);
      SDL_RenderDrawLinesF(m_renderer, first, container.size());
    }
  }
}

template <typename FontKey>
template <typename T>
void basic_renderer<FontKey>::render(const Texture& texture,
                                     const Point<T>& position) noexcept
{
  static_assert(std::is_same_v<T, float> || std::is_same_v<T, int>);

  if constexpr (std::is_same_v<T, int>) {
    const SDL_Rect dst{
        position.x(), position.y(), texture.width(), texture.height()};
    SDL_RenderCopy(m_renderer, texture.get(), nullptr, &dst);
  } else {
    const SDL_FRect dst{position.x(),
                        position.y(),
                        static_cast<float>(texture.width()),
                        static_cast<float>(texture.height())};
    SDL_RenderCopyF(m_renderer, texture.get(), nullptr, &dst);
  }
}

template <typename FontKey>
template <typename T>
void basic_renderer<FontKey>::render(const Texture& texture,
                                     const Rect<T>& destination) noexcept
{
  static_assert(std::is_same_v<T, float> || std::is_same_v<T, int>);

  if constexpr (std::is_same_v<T, int>) {
    SDL_RenderCopy(m_renderer,
                   texture.get(),
                   nullptr,
                   static_cast<const SDL_Rect*>(destination));
  } else {
    SDL_RenderCopyF(m_renderer,
                    texture.get(),
                    nullptr,
                    static_cast<const SDL_FRect*>(destination));
  }
}

template <typename FontKey>
template <typename T>
void basic_renderer<FontKey>::render(const Texture& texture,
                                     const IRect& source,
                                     const Rect<T>& destination) noexcept
{
  static_assert(std::is_same_v<T, float> || std::is_same_v<T, int>);

  if constexpr (std::is_same_v<T, int>) {
    SDL_RenderCopy(m_renderer,
                   texture.get(),
                   static_cast<const SDL_Rect*>(source),
                   static_cast<const SDL_Rect*>(destination));
  } else {
    SDL_RenderCopyF(m_renderer,
                    texture.get(),
                    static_cast<const SDL_Rect*>(source),
                    static_cast<const SDL_FRect*>(destination));
  }
}

template <typename FontKey>
template <typename T>
void basic_renderer<FontKey>::render(const Texture& texture,
                                     const IRect& source,
                                     const Rect<T>& destination,
                                     const double angle) noexcept
{
  static_assert(std::is_same_v<T, float> || std::is_same_v<T, int>);

  if constexpr (std::is_same_v<T, int>) {
    SDL_RenderCopyEx(m_renderer,
                     texture.get(),
                     static_cast<const SDL_Rect*>(source),
                     static_cast<const SDL_Rect*>(destination),
                     angle,
                     nullptr,
                     SDL_FLIP_NONE);
  } else {
    SDL_RenderCopyExF(m_renderer,
                      texture.get(),
                      static_cast<const SDL_Rect*>(source),
                      static_cast<const SDL_FRect*>(destination),
                      angle,
                      nullptr,
                      SDL_FLIP_NONE);
  }
}

template <typename FontKey>
template <typename T>
void basic_renderer<FontKey>::render(const Texture& texture,
                                     const IRect& source,
                                     const Rect<T>& destination,
                                     const double angle,
                                     const Point<T>& center) noexcept
{
  static_assert(std::is_same_v<T, float> || std::is_same_v<T, int>);

  if constexpr (std::is_same_v<T, int>) {
    SDL_RenderCopyEx(m_renderer,
                     texture.get(),
                     static_cast<const SDL_Rect*>(source),
                     static_cast<const SDL_Rect*>(destination),
                     angle,
                     static_cast<const SDL_Point*>(center),
                     SDL_FLIP_NONE);
  } else {
    SDL_RenderCopyExF(m_renderer,
                      texture.get(),
                      static_cast<const SDL_Rect*>(source),
                      static_cast<const SDL_FRect*>(destination),
                      angle,
                      static_cast<const SDL_FPoint*>(center),
                      SDL_FLIP_NONE);
  }
}

template <typename FontKey>
template <typename T>
void basic_renderer<FontKey>::render(const Texture& texture,
                                     const IRect& source,
                                     const Rect<T>& destination,
                                     const double angle,
                                     const Point<T>& center,
                                     const SDL_RendererFlip flip) noexcept
{
  static_assert(std::is_same_v<T, float> || std::is_same_v<T, int>);

  if constexpr (std::is_same_v<T, int>) {
    SDL_RenderCopyEx(m_renderer,
                     texture.get(),
                     static_cast<const SDL_Rect*>(source),
                     static_cast<const SDL_Rect*>(destination),
                     angle,
                     static_cast<const SDL_Point*>(center),
                     flip);
  } else {
    SDL_RenderCopyExF(m_renderer,
                      texture.get(),
                      static_cast<const SDL_Rect*>(source),
                      static_cast<const SDL_FRect*>(destination),
                      angle,
                      static_cast<const SDL_FPoint*>(center),
                      flip);
  }
}

template <typename FontKey>
template <typename T>
void basic_renderer<FontKey>::render_t(const Texture& texture,
                                       const Point<T>& position) noexcept
{
  render(texture, translate(position));
}

template <typename FontKey>
template <typename T>
void basic_renderer<FontKey>::render_t(const Texture& texture,
                                       const Rect<T>& destination) noexcept
{
  render(texture, translate(destination));
}

template <typename FontKey>
template <typename T>
void basic_renderer<FontKey>::render_t(const Texture& texture,
                                       const IRect& source,
                                       const Rect<T>& destination) noexcept
{
  render(texture, source, translate(destination));
}

template <typename FontKey>
template <typename T>
void basic_renderer<FontKey>::render_t(const Texture& texture,
                                       const IRect& source,
                                       const Rect<T>& destination,
                                       const double angle) noexcept
{
  render(texture, source, translate(destination), angle);
}

template <typename FontKey>
template <typename T>
void basic_renderer<FontKey>::render_t(const Texture& texture,
                                       const IRect& source,
                                       const Rect<T>& destination,
                                       const double angle,
                                       const Point<T>& center) noexcept
{
  render(texture, source, translate(destination), angle, center);
}

template <typename FontKey>
template <typename T>
void basic_renderer<FontKey>::render_t(const Texture& texture,
                                       const IRect& source,
                                       const Rect<T>& destination,
                                       const double angle,
                                       const Point<T>& center,
                                       const SDL_RendererFlip flip) noexcept
{
  render(texture, source, translate(destination), angle, center, flip);
}

template <typename FontKey>
void basic_renderer<FontKey>::set_color(const Color& color) noexcept
{
  SDL_SetRenderDrawColor(
      m_renderer, color.red(), color.green(), color.blue(), color.alpha());
}

template <typename FontKey>
void basic_renderer<FontKey>::set_clip(Optional<IRect> area) noexcept
{
  if (area) {
    SDL_RenderSetClipRect(m_renderer, static_cast<const SDL_Rect*>(*area));
  } else {
    SDL_RenderSetClipRect(m_renderer, nullptr);
  }
}

template <typename FontKey>
void basic_renderer<FontKey>::set_viewport(const IRect& viewport) noexcept
{
  SDL_RenderSetViewport(m_renderer, static_cast<const SDL_Rect*>(viewport));
}

template <typename FontKey>
void basic_renderer<FontKey>::set_translation_viewport(
    const FRect& viewport) noexcept
{
  m_translationViewport = viewport;
}

template <typename FontKey>
void basic_renderer<FontKey>::set_blend_mode(BlendMode mode) noexcept
{
  SDL_SetRenderDrawBlendMode(m_renderer, static_cast<SDL_BlendMode>(mode));
}

template <typename FontKey>
void basic_renderer<FontKey>::set_target(const Texture* texture) noexcept
{
  if (texture && texture->is_target()) {
    SDL_SetRenderTarget(m_renderer, texture->get());
  } else {
    SDL_SetRenderTarget(m_renderer, nullptr);
  }
}

template <typename FontKey>
void basic_renderer<FontKey>::set_scale(float xScale, float yScale) noexcept
{
  if (xScale > 0 && yScale > 0) {
    SDL_RenderSetScale(m_renderer, xScale, yScale);
  }
}

template <typename FontKey>
void basic_renderer<FontKey>::set_logical_size(const area_i& size) noexcept
{
  if (size.width > 0 && size.height > 0) {
    SDL_RenderSetLogicalSize(m_renderer, size.width, size.height);
  }
}

template <typename FontKey>
void basic_renderer<FontKey>::set_logical_integer_scale(
    bool useLogicalIntegerScale) noexcept
{
  SDL_RenderSetIntegerScale(m_renderer,
                            detail::convert_bool(useLogicalIntegerScale));
}

template <typename FontKey>
auto basic_renderer<FontKey>::logical_width() const noexcept -> int
{
  int width = 0;
  SDL_RenderGetLogicalSize(m_renderer, &width, nullptr);
  return width;
}

template <typename FontKey>
auto basic_renderer<FontKey>::logical_height() const noexcept -> int
{
  int height = 0;
  SDL_RenderGetLogicalSize(m_renderer, nullptr, &height);
  return height;
}

template <typename FontKey>
auto basic_renderer<FontKey>::x_scale() const noexcept -> float
{
  float xScale = 0;
  SDL_RenderGetScale(m_renderer, &xScale, nullptr);
  return xScale;
}

template <typename FontKey>
auto basic_renderer<FontKey>::y_scale() const noexcept -> float
{
  float yScale = 0;
  SDL_RenderGetScale(m_renderer, nullptr, &yScale);
  return yScale;
}

template <typename FontKey>
auto basic_renderer<FontKey>::clip() const noexcept -> Optional<IRect>
{
  IRect rect;
  SDL_RenderGetClipRect(m_renderer, static_cast<SDL_Rect*>(rect));
  if (!rect.has_area()) {
    return nothing;
  } else {
    return rect;
  }
}

template <typename FontKey>
auto basic_renderer<FontKey>::info() const noexcept
    -> Optional<SDL_RendererInfo>
{
  SDL_RendererInfo info;
  const auto result = SDL_GetRendererInfo(m_renderer, &info);
  if (result == 0) {
    return info;
  } else {
    return nothing;
  }
}

template <typename FontKey>
auto basic_renderer<FontKey>::output_width() const noexcept -> int
{
  int width = 0;
  SDL_GetRendererOutputSize(m_renderer, &width, nullptr);
  return width;
}

template <typename FontKey>
auto basic_renderer<FontKey>::output_height() const noexcept -> int
{
  int height = 0;
  SDL_GetRendererOutputSize(m_renderer, nullptr, &height);
  return height;
}

template <typename FontKey>
auto basic_renderer<FontKey>::output_size() const noexcept -> area_i
{
  int width = 0;
  int height = 0;
  SDL_GetRendererOutputSize(m_renderer, &width, &height);
  return {width, height};
}

template <typename FontKey>
auto basic_renderer<FontKey>::blend_mode() const noexcept -> BlendMode
{
  SDL_BlendMode mode;
  SDL_GetRenderDrawBlendMode(m_renderer, &mode);
  return static_cast<BlendMode>(mode);
}

template <typename FontKey>
auto basic_renderer<FontKey>::flags() const noexcept -> Uint32
{
  SDL_RendererInfo info;
  SDL_GetRendererInfo(m_renderer, &info);
  return info.flags;
}

template <typename FontKey>
auto basic_renderer<FontKey>::vsync_enabled() const noexcept -> bool
{
  return flags() & SDL_RENDERER_PRESENTVSYNC;
}

template <typename FontKey>
auto basic_renderer<FontKey>::accelerated() const noexcept -> bool
{
  return flags() & SDL_RENDERER_ACCELERATED;
}

template <typename FontKey>
auto basic_renderer<FontKey>::software_based() const noexcept -> bool
{
  return flags() & SDL_RENDERER_SOFTWARE;
}

template <typename FontKey>
auto basic_renderer<FontKey>::supports_target_textures() const noexcept -> bool
{
  return flags() & SDL_RENDERER_TARGETTEXTURE;
}

template <typename FontKey>
auto basic_renderer<FontKey>::using_integer_logical_scaling() const noexcept
    -> bool
{
  return SDL_RenderGetIntegerScale(m_renderer);
}

template <typename FontKey>
auto basic_renderer<FontKey>::clipping_enabled() const noexcept -> bool
{
  return SDL_RenderIsClipEnabled(m_renderer);
}

template <typename FontKey>
auto basic_renderer<FontKey>::color() const noexcept -> Color
{
  Uint8 red = 0, green = 0, blue = 0, alpha = 0;
  SDL_GetRenderDrawColor(m_renderer, &red, &green, &blue, &alpha);
  return {red, green, blue, alpha};
}

template <typename FontKey>
auto basic_renderer<FontKey>::text_blended(gsl::czstring text,
                                           const Font& font) const noexcept
    -> std::unique_ptr<Texture>
{
  return render_text(text, [this, &font](gsl::czstring text) noexcept {
    return TTF_RenderText_Blended(
        font.get(), text, static_cast<SDL_Color>(color()));
  });
}

template <typename FontKey>
auto basic_renderer<FontKey>::text_blended_wrapped(
    gsl::czstring text,
    Uint32 wrap,
    const Font& font) const noexcept -> std::unique_ptr<Texture>
{
  return render_text(text, [this, &font, wrap](gsl::czstring text) noexcept {
    return TTF_RenderText_Blended_Wrapped(
        font.get(), text, static_cast<SDL_Color>(color()), wrap);
  });
}

template <typename FontKey>
auto basic_renderer<FontKey>::text_shaded(gsl::czstring text,
                                          const Color& bg,
                                          const Font& font) const noexcept
    -> std::unique_ptr<Texture>
{
  return render_text(text, [this, &font, &bg](gsl::czstring text) noexcept {
    return TTF_RenderText_Shaded(font.get(),
                                 text,
                                 static_cast<SDL_Color>(color()),
                                 static_cast<SDL_Color>(bg));
  });
}

template <typename FontKey>
auto basic_renderer<FontKey>::text_solid(gsl::czstring text,
                                         const Font& font) const noexcept
    -> std::unique_ptr<Texture>
{
  return render_text(text, [this, &font](gsl::czstring text) noexcept {
    return TTF_RenderText_Solid(
        font.get(), text, static_cast<SDL_Color>(color()));
  });
}

template <typename FontKey>
auto basic_renderer<FontKey>::font(const FontKey& key) noexcept
    -> std::shared_ptr<Font>
{
  if (m_fonts.count(key)) {
    return m_fonts.at(key);
  } else {
    return nullptr;
  }
}

template <typename FontKey>
auto basic_renderer<FontKey>::has_font(const FontKey& key) const noexcept
    -> bool
{
  return m_fonts.count(key);
}

template <typename FontKey>
auto basic_renderer<FontKey>::viewport() const noexcept -> IRect
{
  IRect viewport;
  SDL_RenderGetViewport(m_renderer, static_cast<SDL_Rect*>(viewport));
  return viewport;
}

template <typename FontKey>
auto basic_renderer<FontKey>::to_string() const -> std::string
{
  const auto address = detail::address_of(this);
  const auto owidth = std::to_string(output_width());
  const auto oheight = std::to_string(output_height());
  return "[Renderer@" + address + " | Output width: " + owidth +
         ", Output height: " + oheight + "]";
}

template <typename FontKey>
auto basic_renderer<FontKey>::translation_viewport() const noexcept
    -> const FRect&
{
  return m_translationViewport;
}

template <typename FontKey>
auto basic_renderer<FontKey>::get() const noexcept -> SDL_Renderer*
{
  return m_renderer;
}

template <typename FontKey>
basic_renderer<FontKey>::operator SDL_Renderer*() noexcept
{
  return m_renderer;
}

template <typename FontKey>
basic_renderer<FontKey>::operator const SDL_Renderer*() const noexcept
{
  return m_renderer;
}

template <typename FontKey>
auto basic_renderer<FontKey>::render_drivers() noexcept -> int
{
  return SDL_GetNumRenderDrivers();
}

template <typename FontKey>
auto basic_renderer<FontKey>::video_drivers() noexcept -> int
{
  return SDL_GetNumVideoDrivers();
}

template <typename FontKey>
auto basic_renderer<FontKey>::driver_info(int index) noexcept
    -> Optional<SDL_RendererInfo>
{
  SDL_RendererInfo info;
  const auto result = SDL_GetRenderDriverInfo(index, &info);
  if (result == 0) {
    return info;
  } else {
    return nothing;
  }
}

}  // namespace centurion

#ifdef CENTURION_HEADER_ONLY
#include "texture.cpp"
#endif  // CENTURION_HEADER_ONLY

#endif  // CENTURION_VIDEO_HEADER