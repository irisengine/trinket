////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "iris/core/quaternion.h"
#include "iris/core/vector3.h"
#include "iris/graphics/render_entity.h"
#include "iris/graphics/scene.h"
#include "iris/physics/character_controller.h"
#include "iris/physics/physics_system.h"

#include "game_object.h"

namespace trinket
{

class Player : public GameObject
{
  public:
    Player(iris::Scene &scene, iris::PhysicsSystem *ps);
    ~Player() override = default;

    void update() override;

    void set_orientation(const iris::Quaternion &orientation);
    void set_walk_direction(const iris::Vector3 &direction);
    iris::Vector3 position() const;

  private:
    iris::RenderEntity *render_entity_;
    iris::CharacterController *character_controller_;
};

}
