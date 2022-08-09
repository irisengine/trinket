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

class Enemy : public GameObject, Subscriber, Publisher
{
  public:
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
    ~Enemy() override = default;

    void update(std::chrono::microseconds elapsed) override;

    void handle_message(MessageType message_type, const std::any &data) override;
    iris::Vector3 position() const;

  private:
    iris::ScriptRunner script_;
    iris::SingleEntity *render_entity_;
    iris::SingleEntity *health_bar_;
    std::unique_ptr<iris::AnimationController> animation_controller_;
    CharacterController *character_controller_;
    const Player *player_;
    const ThirdPersonCamera *camera_;
    std::chrono::system_clock::time_point hit_cooldown_;
    iris::Vector3 health_bar_scale_;
    float health_;
    bool is_dead_;
};

}
