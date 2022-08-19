////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <any>
#include <chrono>
#include <cstdint>
#include <map>

#include "iris/core/camera.h"
#include "iris/events/event.h"
#include "iris/physics/physics_system.h"

#include "game_object.h"
#include "message_type.h"
#include "player.h"
#include "subscriber.h"

namespace trinket
{

/**
 * Implementation of GameObject for a third person camera i.e a camera that always follows and looks at the player.
 */
class ThirdPersonCamera : public GameObject, Subscriber
{
  public:
    /**
     * Create a new ThirdPersonCamera.
     *
     * @param player
     *   Pointer to player to follow.
     *
     * @param width
     *   Screen width.
     *
     * @param height
     *   Screen height.
     *
     * @param ps
     *   Physics system.
     */
    ThirdPersonCamera(Player *player, std::uint32_t width, std::uint32_t height, iris::PhysicsSystem *ps);

    /**
     * Update object.
     *
     * @param elapsed
     *   Time since last update.
     */
    void update(std::chrono::microseconds) override;

    /**
     * Get the engine camera object.
     *
     * @returns
     *   Pointer to engine camera.
     */
    iris::Camera *camera();
    /**
     * Get the engine camera object.
     *
     * @returns
     *   Pointer to engine camera.
     */
    const iris::Camera *camera() const;

  protected:
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
    /** Pointer to player to follow. */
    Player *player_;

    /** Engine camera. */
    iris::Camera camera_;

    /** Map of pressed keys. */
    std::map<iris::Key, iris::KeyState> key_map_;

    /** Camera azimuth. */
    float azimuth_;

    /** Camera altitude. */
    float altitude_;

    /** Distance to player. */
    float camera_distance_;

    /** Physics system. */
    iris::PhysicsSystem *ps_;
};

}
