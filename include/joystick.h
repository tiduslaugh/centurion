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
 * @brief Provides the joystick API.
 *
 * @file joystick.h
 * @author Albin Johansson
 * @copyright MIT License
 * @date 2019-2020
 */

#ifndef CENTURION_JOYSTICK_HEADER
#define CENTURION_JOYSTICK_HEADER

#include <SDL_joystick.h>

#include "centurion_api.h"
#include "centurion_utils.h"

namespace centurion {

/**
 * A type alias for SDL_JoystickID.
 *
 * @since 4.0.0
 */
using JoystickID = SDL_JoystickID;

/**
 * @brief Represents various types of joysticks.
 *
 * @invariant A successfully created `Joystick` instance is always viable.
 * Which means that `SDL_JoystickGetAttached(SDL_Joystick*)` always returns
 * `true` for the internal `SDL_Joystick*`.
 *
 * @class Joystick
 * @headerfile joystick.h
 * @since 4.2.0
 * @see SDL_Joystick
 */
class Joystick final {
 public:
  /**
   * @brief Mirrors the SDL_JoystickPowerLevel enum.
   *
   * @enum Power
   * @headerfile joystick.h
   * @since 4.2.0
   */
  enum class Power {
    Unknown = SDL_JOYSTICK_POWER_UNKNOWN, /**< Unknown power level. */
    Empty = SDL_JOYSTICK_POWER_EMPTY,     /**< Indicates <= 5% power. */
    Low = SDL_JOYSTICK_POWER_LOW,         /**< Indicates <= 20% power. */
    Medium = SDL_JOYSTICK_POWER_MEDIUM,   /**< Indicates <= 70% power. */
    Full = SDL_JOYSTICK_POWER_FULL,       /**< Indicates <= 100% power. */
    Wired = SDL_JOYSTICK_POWER_WIRED,     /**< Wired joystick, no need to
                                           * worry about power. */
    Max = SDL_JOYSTICK_POWER_MAX          /**< Maximum power level. */
  };

  /**
   * @brief Creates a `Joystick` instance based on a device index.
   * @warning The device index is not the same as the instance ID used to
   * identify the joystick in future events.
   * @param deviceIndex refers to the N'th joystick that is currently
   * recognized by SDL.
   * @throws CenturionException if the joystick cannot be created.
   * @since 4.2.0
   */
  CENTURION_API explicit Joystick(int deviceIndex);

  /**
   * @brief Creates a `Joystick` instance based on an existing `SDL_Joystick*`.
   * @pre `joystick` must not be null.
   * @param joystick a pointer to the `SDL_Joystick` that will be claimed.
   * @throws CenturionException if the joystick cannot be created.
   * @since 4.2.0
   */
  CENTURION_API explicit Joystick(Owner<SDL_Joystick*> joystick);

  CENTURION_API ~Joystick() noexcept;

  /**
   * @brief Creates and returns a unique pointer to a `Joystick` instance.
   * @invariant This method never returns a null pointer.
   * @param deviceIndex refers to the N'th joystick that is currently
   * recognized by SDL.
   * @return a unique pointer to a `Joystick` instance.
   * @throws CenturionException if the joystick cannot be created.
   * @since 4.2.0
   */
  CENTURION_NODISCARD
  CENTURION_API static UniquePtr<Joystick> unique(int deviceIndex);

  /**
   * @brief Creates and returns a unique pointer to a `Joystick` instance.
   * @pre `joystick` must not be null.
   * @invariant This method never returns a null pointer.
   * @param joystick a pointer to the `SDL_Joystick` that will be claimed.
   * @return a unique pointer to a `Joystick` instance.
   * @throws CenturionException if the joystick cannot be created.
   * @since 4.2.0
   */
  CENTURION_NODISCARD
  CENTURION_API static UniquePtr<Joystick> unique(
      Owner<SDL_Joystick*> joystick);

  /**
   * @brief Creates and returns a shared pointer to a `Joystick` instance.
   * @invariant This method never returns a null pointer.
   * @param deviceIndex refers to the N'th joystick that is currently
   * recognized by SDL.
   * @return a shared pointer to a `Joystick` instance.
   * @throws CenturionException if the joystick cannot be created.
   * @since 4.2.0
   */
  CENTURION_NODISCARD
  CENTURION_API static SharedPtr<Joystick> shared(int deviceIndex);

  /**
   * @brief Creates and returns a shared pointer to a `Joystick` instance.
   * @pre `joystick` must not be null.
   * @invariant This method never returns a null pointer.
   * @param joystick a pointer to the `SDL_Joystick` that will be claimed.
   * @return a shared pointer to a `Joystick` instance.
   * @throws CenturionException if the joystick cannot be created.
   * @since 4.2.0
   */
  CENTURION_NODISCARD
  CENTURION_API static SharedPtr<Joystick> shared(
      Owner<SDL_Joystick*> joystick);

  /**
   * @brief Updates the state of all open joysticks.
   * @note This is done automatically by the event loop if any joystick
   * events are enabled.
   * @since 4.2.0
   */
  CENTURION_API static void update() noexcept;

  /**
   * @brief Returns a pointer to an `SDL_Joystick` associated with the ID.
   * @param id the joystick ID associated with the desired joystick.
   * @return a pointer to an `SDL_Joystick` instance if there is a joystick
   * associated with the supplied ID; null otherwise.
   * @since 4.2.0
   */
  CENTURION_NODISCARD
  CENTURION_API static SDL_Joystick* from_instance_id(JoystickID id) noexcept;

  /**
   * @brief Returns the current position of the specified axis.
   * @details Most modern joysticks let the X-axis be represented by 0
   * and the Y-axis by 1. To account for jitter, it may be necessary to impose
   * some kind of tolerance on the returned value.
   * @note Some joysticks use axes 2 and 3 for extra buttons.
   * @param axis the ID of the axis to query.
   * @return a 16-bit signed integer that represents the position of the
   * specified axis; `nothing` if something goes wrong.
   * @since 4.2.0
   */
  CENTURION_NODISCARD
  CENTURION_API Optional<Sint16> axis_pos(unsigned int axis) const noexcept;

  /**
   * @brief Returns the current power level of the joystick.
   * @return a `Joystick::Power` value that represents the current power level.
   * @since 4.2.0
   */
  CENTURION_NODISCARD
  CENTURION_API Power power() const noexcept;

  /**
   * @brief Returns a pointer to the associated `SDL_Joystick`.
   * @warning Use of this method is not recommended, since it purposefully
   * breaks const-correctness. However, it can be useful since many SDL calls
   * use non-const pointers even when no change will be applied. Don't take
   * ownership of the returned pointer, or bad things will happen.
   * @return a pointer to the internal `SDL_Joystick`.
   * @since 4.2.0
   */
  CENTURION_NODISCARD SDL_Joystick* get() const noexcept { return m_joystick; }

 private:
  SDL_Joystick* m_joystick;
};

}  // namespace centurion

#ifdef CENTURION_HEADER_ONLY
#include "joystick.cpp"
#endif  // CENTURION_HEADER_ONLY

#endif  // CENTURION_JOYSTICK_HEADER