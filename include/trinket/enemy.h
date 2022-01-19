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
#include "iris/graphics/render_entity.h"
#include "iris/graphics/scene.h"
#include "iris/physics/character_controller.h"
#include "iris/physics/physics_system.h"
#include "iris/physics/rigid_body.h"
#include "iris/scripting/script_runner.h"

#include "game_object.h"
#include "message_type.h"
#include "subscriber.h"

namespace trinket
{

class Enemy : public GameObject, Subscriber
{
  public:
    Enemy(
        iris::PhysicsSystem *ps,
        const std::string &script_file,
        iris::RenderEntity *render_entity,
        const std::vector<iris::Animation> &animations);
    ~Enemy() override = default;

    void update() override;

    void handle_message(MessageType message_type, const std::any &data) override;

  private:
    iris::ScriptRunner script_;
    iris::RenderEntity *render_entity_;
    std::unique_ptr<iris::AnimationController> animation_controller_;
    iris::CharacterController *character_controller_;
};

}
