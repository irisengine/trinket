////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <chrono>

#include "iris/core/vector3.h"
#include "iris/physics/basic_character_controller.h"
#include "iris/physics/physics_system.h"

namespace trinket
{

/**
 * Extension of iris::BasicCharacterController that allows the controller to be shunted in a direction.
 */
class CharacterController : public iris::BasicCharacterController
{
  public:
    /**
     * Construct a new CharacterController.
     *
     * @param ps
     *   Physics system.
     *
     * @param speed
     *   Speed of character.
     *
     * @param width
     *   Width of character capsule.
     *
     * @param height
     *   Height of character capsule.
     */
    CharacterController(iris::PhysicsSystem *ps, float speed, float width, float height, float float_height);

    /**
     * Set the direction the character is walking. Should be a normalised vector.
     *
     * @param direction
     *   Direction character is moving.
     */
    void set_movement_direction(const iris::Vector3 &direction) override;

    /**
     * Called to update controller.
     *
     * @param ps
     *   Physics system that created controller.
     *
     * @param delta
     *   Time since last update.
     */
    void update(iris::PhysicsSystem *ps, std::chrono::milliseconds delta) override;

    /**
     * Shunt the character in a direction.
     *
     * @param direction
     *   Direction to shunt.
     *
     * @param distance
     *   Distance to shunt.
     *
     * @param time
     *   The time to wait before allowing character to be shunted again.
     */
    void shunt(const iris::Vector3 direction, float distance, std::chrono::milliseconds time);

  private:
    /** Flag indicating if character is currently being shunted. */
    bool is_being_shunted_;

    /** Time when character can be shunted again. */
    std::chrono::system_clock::time_point shunt_end_;

    /** Store of movement direction. */
    iris::Vector3 saved_movement_direction_;

    /** Store of speed. */
    float saved_speed_;
};

}
