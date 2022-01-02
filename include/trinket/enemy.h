////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <any>
#include <chrono>

#include "iris/core/vector3.h"
#include "iris/graphics/render_entity.h"
#include "iris/graphics/scene.h"
#include "iris/physics/physics_system.h"
#include "iris/physics/rigid_body.h"

#include "game_object.h"
#include "message_type.h"
#include "subscriber.h"

namespace trinket
{

class Enemy : public GameObject, Subscriber
{
  public:
    Enemy(const iris::Vector3 &position, iris::Scene &scene, iris::PhysicsSystem *ps);
    ~Enemy() override = default;

    void update() override;

    void handle_message(MessageType message_type, const std::any &data) override;

  private:
    iris::RenderEntity *render_entity_;
    iris::RigidBody *rigid_body_;
    std::chrono::system_clock::time_point hit_cooldown_;
};

}
