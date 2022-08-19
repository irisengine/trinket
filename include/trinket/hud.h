////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <any>
#include <chrono>
#include <cstdint>

#include "iris/core/camera.h"
#include "iris/graphics/cube_map.h"
#include "iris/graphics/render_pass.h"
#include "iris/graphics/render_target.h"
#include "iris/graphics/scene.h"
#include "iris/graphics/single_entity.h"

#include "game_object.h"
#include "message_type.h"
#include "subscriber.h"

namespace trinket
{

/**
 * Game object for displaying a HUD (health and xp).
 */
class HUD : public GameObject, Subscriber
{
  public:
    /**
     * Create a new HUD object.
     *
     * @param starting_health
     *   The amount of initial health.
     *
     * @param width
     *   Screen width.
     *
     * @param height
     *   Screen height.
     *
     * @param scene
     *   Scene to create HUD elements in.
     */
    HUD(float starting_health, std::uint32_t width, std::uint32_t height, iris::Scene *scene);

    /**
     * Update object.
     *
     * @param elapsed
     *   Time since last update.
     */
    void update(std::chrono::microseconds) override;

    /**
     * Message handler.
     *
     * @param message_type
     *   Type of message being sent.
     *
     * @param data
     *   Any data for the message.
     */
    void handle_message(MessageType message_type, const std::any &data) override;

  private:
    /** Entity for health bar. */
    iris::SingleEntity *health_bar_;

    /** Entity for xp bar. */
    iris::SingleEntity *level_progress_bar_;

    /** Camera for billboards. */
    iris::Camera camera_;

    /** Screen width. */
    float width_;

    /** Screen height. */
    float height_;

    /** Starting health. */
    float starting_health_;
};

}
