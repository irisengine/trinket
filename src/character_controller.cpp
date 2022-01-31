////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include "character_controller.h"

#include "iris/core/vector3.h"
#include "iris/physics/basic_character_controller.h"
#include "iris/physics/physics_system.h"

namespace trinket
{

CharacterController::CharacterController(
    iris::PhysicsSystem *ps,
    float speed,
    float width,
    float height,
    float float_height)
    : iris::BasicCharacterController(ps, speed, width, height, float_height)
    , is_being_shunted_(false)
    , shunt_end_(std::chrono::system_clock::now())
    , shunt_distance_(0.0f)
    , saved_movement_direction_()
    , saved_speed_(0.0)
{
}

void CharacterController::set_movement_direction(const iris::Vector3 &direction)
{
    if (!is_being_shunted_)
    {
        iris::BasicCharacterController::set_movement_direction(direction);
    }
    else
    {
        saved_movement_direction_ = direction;
    }
}

void CharacterController::update(iris::PhysicsSystem *ps, std::chrono::milliseconds delta)
{
    iris::BasicCharacterController::update(ps, delta);

    if (is_being_shunted_ && (std::chrono::system_clock::now() > shunt_end_))
    {
        movement_direction_ = saved_movement_direction_;
        speed_ = saved_speed_;
        is_being_shunted_ = false;
    }
}

void CharacterController::shunt(const iris::Vector3 direction, float distance, std::chrono::milliseconds time)
{
    if (!is_being_shunted_)
    {
        saved_movement_direction_ = movement_direction_;
        saved_speed_ = speed_;

        movement_direction_ = direction;
        speed_ = distance / (static_cast<float>(time.count()) / 1000.0f);

        shunt_end_ = std::chrono::system_clock::now() + time;
        shunt_distance_ = distance;
        is_being_shunted_ = true;
    }
}

}
