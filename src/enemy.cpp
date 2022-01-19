////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include "enemy.h"

#include <any>
#include <memory>
#include <string>

#include "iris/core/resource_loader.h"
#include "iris/core/vector3.h"
#include "iris/graphics/animation/animation_layer.h"
#include "iris/graphics/render_entity.h"
#include "iris/graphics/scene.h"
#include "iris/log/log.h"
#include "iris/physics/physics_system.h"
#include "iris/physics/rigid_body.h"
#include "iris/scripting/lua/lua_script.h"

#include "message_type.h"

using namespace std::literals::chrono_literals;

namespace trinket
{

struct EnemyInterop
{
    iris::CharacterController *character_controller;
};

Enemy::Enemy(
    iris::PhysicsSystem *ps,
    const std::string &script_file,
    iris::RenderEntity *render_entity,
    const std::vector<iris::Animation> &animations)
    : script_(std::make_unique<iris::LuaScript>(script_file, iris::LuaScript::LoadFile{}))
    , render_entity_(render_entity)
    , animation_controller_(nullptr)
    , character_controller_(nullptr)
{
    script_.execute("init");

    animation_controller_ = std::make_unique<iris::AnimationController>(
        animations,
        std::vector<iris::AnimationLayer>{{{{"Dance", "Dance", 0ms}}, "Dance"}},
        render_entity_->skeleton());

    character_controller_ = ps->create_character_controller();
    character_controller_->reposition(render_entity_->position(), {});
}

void Enemy::update()
{
    script_.execute("update");

    character_controller_->set_walk_direction({-1.0f, 0.0f, 0.0f});
    render_entity_->set_position(character_controller_->position());
    animation_controller_->update();
}

void Enemy::handle_message(MessageType message_type, const std::any &data)
{
}

}
