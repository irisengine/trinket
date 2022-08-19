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
#include <unordered_map>
#include <vector>

#include "iris/core/quaternion.h"
#include "iris/core/vector3.h"
#include "iris/graphics/animation/animation_controller.h"
#include "iris/graphics/render_pipeline.h"
#include "iris/graphics/scene.h"
#include "iris/graphics/single_entity.h"
#include "iris/physics/physics_system.h"
#include "iris/physics/rigid_body.h"

#include "character_controller.h"
#include "game_object.h"
#include "message_type.h"
#include "publisher.h"
#include "subscriber.h"

namespace trinket
{

/**
 * Implementation of GameObject for Player.
 */
class Player : public GameObject, Publisher, Subscriber
{
  public:
    /**
     * Construct a new player object.
     *
     * @param scene
     *   Scene player will be added to.
     *
     * @param ps
     *   Physics system.
     *
     * @param start_position
     *   World space coords of player spawn.
     *
     * @param render_pipeline
     *   Render pipeline to use for player.
     */
    Player(
        iris::Scene *scene,
        iris::PhysicsSystem *ps,
        const iris::Vector3 &start_position,
        iris::RenderPipeline &render_pipeline);

    /**
     * Update object.
     *
     * @param elapsed
     *   Time since last update.
     */
    void update(std::chrono::microseconds) override;

    /**
     * Set orientation oif player.
     *
     * @param orientation
     *   New orientation.
     */
    void set_orientation(const iris::Quaternion &orientation);

    /**
     * Set walk direction of player.
     *
     * @param direction
     *   New walk direction of player.
     */
    void set_walk_direction(const iris::Vector3 &direction);

    /**
     * Get position ofm player.
     *
     * @returns
     *   Player position.
     */
    iris::Vector3 position() const;

    /**
     * Get player rigid body.
     *
     * @returns
     *   Player rigid body.
     */
    const iris::RigidBody *rigid_body() const;

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
    /**
     * Internal struct for handling sub mesh data.
     */
    struct SubMesh
    {
        std::string bone_attach;
        iris::Matrix4 transform;
        iris::Vector3 offset;
    };

    /** Collection of entities for player. */
    std::vector<iris::SingleEntity *> render_entities_;

    /** Character controller. */
    CharacterController *character_controller_;

    /** Sword rigid body. */
    iris::RigidBody *sword_body_;

    /** Flag indicating if player is attacking. */
    bool attacking_;

    /** Time point whe player will have stopped attacking. */
    std::chrono::system_clock::time_point attack_stop_;

    /** Duration of player attack. */
    std::chrono::milliseconds attack_duration_;

    /** Physics system. */
    iris::PhysicsSystem *ps_;

    /** Time point to stop animation blending. */
    std::chrono::system_clock::time_point blend_stop_;

    /** Duration of animation blend. */
    std::chrono::milliseconds blend_time_;

    /** Flag indicating if animation blending is occurring. */
    bool blending_;

    /** Animation controller. */
    std::unique_ptr<iris::AnimationController> animation_controller_;

    /** Tracker for number of move key presses. */
    std::uint32_t move_key_pressed_;

    /** Player health. */
    float health_;

    /** Player skeleton. */
    iris::Skeleton *skeleton_;

    /** Map of entities to sub mesh data. */
    std::unordered_map<iris::SingleEntity *, SubMesh> sub_meshes_;

    /** Render entity for player sword. */
    iris::SingleEntity *sword_entity_;

    /** Player xp. */
    std::uint32_t xp_;

    /** Amount of xp required to level up. */
    std::uint32_t next_level_;
};

}
