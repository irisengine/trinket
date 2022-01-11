////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <any>
#include <chrono>
#include <map>
#include <memory>
#include <string>

#include "iris/core/quaternion.h"
#include "iris/core/vector3.h"
#include "iris/graphics/animation/animation_controller.h"
#include "iris/graphics/render_entity.h"
#include "iris/graphics/scene.h"
#include "iris/physics/character_controller.h"
#include "iris/physics/physics_system.h"
#include "iris/physics/rigid_body.h"

#include "game_object.h"
#include "message_type.h"
#include "publisher.h"
#include "subscriber.h"

namespace trinket
{

class Player : public GameObject, Publisher, Subscriber
{
  public:
    Player(iris::Scene &scene, iris::PhysicsSystem *ps, const iris::Vector3 &start_position);
    ~Player() override = default;

    void update() override;

    void set_orientation(const iris::Quaternion &orientation);
    void set_walk_direction(const iris::Vector3 &direction);
    iris::Vector3 position() const;
    const iris::RigidBody *rigid_body() const;

    void handle_message(MessageType message_type, const std::any &data) override;

  private:
    iris::RenderEntity *render_entity_;
    iris::CharacterController *character_controller_;
    iris::RenderEntity *sword_;
    iris::RigidBody *sword_body_;
    bool attacking_;
    std::chrono::system_clock::time_point attack_stop_;
    std::chrono::milliseconds attack_duration_;
    iris::PhysicsSystem *ps_;
    std::chrono::system_clock::time_point blend_stop_;
    std::chrono::milliseconds blend_time_;
    bool blending_;
    std::unique_ptr<iris::AnimationController> animation_controller_;
    std::uint32_t move_key_pressed_;
};

}
