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

class CharacterController : public iris::BasicCharacterController
{
  public:
    CharacterController(iris::PhysicsSystem *ps, float speed, float width, float height, float float_height);
    ~CharacterController() override = default;

    void set_movement_direction(const iris::Vector3 &direction) override;

    void update(iris::PhysicsSystem *ps, std::chrono::milliseconds delta) override;

    void shunt(const iris::Vector3 direction, float distance, std::chrono::milliseconds time);

  private:
    bool is_being_shunted_;
    std::chrono::system_clock::time_point shunt_end_;
    float shunt_distance_;
    iris::Vector3 saved_movement_direction_;
    float saved_speed_;
};

}
