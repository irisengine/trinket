////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <any>
#include <chrono>
#include <memory>
#include <string>
#include <vector>

#include "iris/core/vector3.h"
#include "iris/graphics/animation/animation.h"
#include "iris/graphics/animation/animation_controller.h"
#include "iris/graphics/scene.h"
#include "iris/graphics/single_entity.h"
#include "iris/physics/physics_system.h"
#include "iris/physics/rigid_body.h"
#include "iris/scripting/script_runner.h"

#include "character_controller.h"
#include "game_object.h"
#include "message_type.h"
#include "player.h"
#include "publisher.h"
#include "subscriber.h"
#include "third_person_camera.h"

namespace trinket
{

/**
 * Implementation of GameObject for an enemy, logic is driven via a lua script.
 */
class Enemy : public GameObject, Subscriber, Publisher
{
  public:
    /**
     * Construct a new Enemy.
     *
     * @param ps
     *   Physis system.
     *
     * @param script_file
     *   Path to script resource.
     *
     * @param render_entity
     *   Render entity of enemy.
     *
     * @param health_bar
     *   Render entity o health bar.
     *
     * @param animations
     *   Collection of animations for enemy.
     *
     * @param bounds_min
     *   Minimum bounds of enemy patrol zone.
     *
     * @param bounds_max
     *   Maximum bounds of enemy patrol zone.
     *
     * @param player
     *   Pointer to player object.
     *
     * @param camera
     *   Pointer to camera object.
     */
    Enemy(
        iris::PhysicsSystem *ps,
        const std::string &script_file,
        iris::SingleEntity *render_entity,
        iris::SingleEntity *health_bar,
        std::vector<iris::Animation> animations,
        const iris::Vector3 &bounds_min,
        const iris::Vector3 &bounds_max,
        const Player *player,
        const ThirdPersonCamera *camera);

    /**
     * Update object.
     *
     * @param elapsed
     *   Time since last update.
     */
    void update(std::chrono::microseconds elapsed) override;

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

    /**
     * Get position on enemy.
     *
     * @returns
     *   Enemy position.
     */
    iris::Vector3 position() const;

  private:
    /** Script runner for enemy AI. */
    iris::ScriptRunner script_;

    /** Render entity for enemy. */
    iris::SingleEntity *render_entity_;

    /** Render entity for health bar. */
    iris::SingleEntity *health_bar_;

    /** Animation controller. */
    std::unique_ptr<iris::AnimationController> animation_controller_;

    /** Character controller. */
    CharacterController *character_controller_;

    /** Pointer to player object. */
    const Player *player_;

    /** Pointer to camera object. */
    const ThirdPersonCamera *camera_;

    /** Cooldown timer between being hit. */
    std::chrono::system_clock::time_point hit_cooldown_;

    /** Scale of health bar entity. */
    iris::Vector3 health_bar_scale_;

    /** Health of enemy. */
    float health_;

    /** Flag indicating if enemy is dead. */
    bool is_dead_;
};

}
