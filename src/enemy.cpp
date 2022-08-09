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
#include "iris/graphics/scene.h"
#include "iris/graphics/single_entity.h"
#include "iris/log/log.h"
#include "iris/physics/physics_system.h"
#include "iris/physics/rigid_body.h"
#include "iris/scripting/lua/lua_script.h"

#include "character_controller.h"
#include "message_type.h"
#include "player.h"

using namespace std::literals::chrono_literals;

namespace trinket
{

Enemy::Enemy(
    iris::PhysicsSystem *ps,
    const std::string &script_file,
    iris::SingleEntity *render_entity,
    iris::SingleEntity *health_bar,
    std::vector<iris::Animation> animations,
    const iris::Vector3 &bounds_min,
    const iris::Vector3 &bounds_max,
    const Player *player,
    const ThirdPersonCamera *camera)
    : script_(std::make_unique<iris::LuaScript>(script_file, iris::LuaScript::LoadFile{}))
    , render_entity_(render_entity)
    , health_bar_(health_bar)
    , animation_controller_(nullptr)
    , character_controller_(nullptr)
    , player_(player)
    , camera_(camera)
    , hit_cooldown_(std::chrono::system_clock::now())
    , health_bar_scale_(health_bar_->scale())
    , health_(100.0f)
    , is_dead_(false)
{
    script_.execute("init", bounds_min, bounds_max);

    auto find = std::find_if(std::begin(animations), std::end(animations), [](const iris::Animation &animation) {
        return animation.name() == "Death_Back";
    });
    find->set_playback_type(iris::PlaybackType::SINGLE);

    animation_controller_ = std::make_unique<iris::AnimationController>(
        animations,
        std::vector<iris::AnimationLayer>{
            {{{"Walk", "Walk", 0ms},
              {"Walk", "Bite_Front", 500ms},
              {"Walk", "Death_Back", 500ms},
              {"Bite_Front", "Death_Back", 500ms},
              {"Death_Back", "Death_Back", 0ms},
              {"Bite_Front", "Walk", 500ms},
              {"Bite_Front", "Bite_Front", 0ms}},
             "Walk"}},
        render_entity_->skeleton());

    character_controller_ = ps->create_character_controller<CharacterController>(ps, 1.0f, 1.0f, 0.5f, 2.0f);
    character_controller_->reposition(render_entity_->position(), {});

    subscribe(MessageType::WEAPON_COLLISION);
}

void Enemy::update(std::chrono::microseconds elapsed)
{
    if (!is_dead_)
    {
        script_.execute(
            "update",
            render_entity_->position(),
            player_->position(),
            static_cast<std::int32_t>(elapsed.count()),
            health_);

        static const iris::Vector3 offset{0.0f, -2.0f, 0.0f};

        character_controller_->set_movement_direction(script_.execute<iris::Vector3>("get_walk_direction"));
        render_entity_->set_orientation(script_.execute<iris::Quaternion>("get_orientation"));
        render_entity_->set_position(character_controller_->position() + offset);

        iris::Transform billboard_transform{iris::Matrix4::invert(camera_->camera()->view())};
        billboard_transform.set_translation(render_entity_->position() + iris::Vector3{0.0f, 3.0f, 0.0f});
        billboard_transform.set_scale(health_bar_scale_);
        health_bar_->set_transform(billboard_transform.matrix());

        if (const auto [change, animation] = script_.execute<bool, std::string>("get_animation_change"); change)
        {
            LOG_DEBUG("enemy", "new animation: {}", animation);
            animation_controller_->play(0u, animation);
        }

        if (const auto attack = script_.execute<bool>("attack_player"); attack)
        {
            publish(MessageType::ENEMY_ATTACK, {1.0f});
        }

        if (health_ <= 0.0f)
        {
            character_controller_->reposition(render_entity_->position(), {});
            is_dead_ = true;

            publish(MessageType::KILLED_ENEMY, {this});
        }
    }

    animation_controller_->update();
}

void Enemy::handle_message(MessageType message_type, const std::any &data)
{
    switch (message_type)
    {
        case MessageType::WEAPON_COLLISION:
        {
            const auto &[body, pos] = std::any_cast<std::tuple<iris::RigidBody *, iris::Vector3>>(data);
            if (body == character_controller_->rigid_body())
            {
                if (std::chrono::system_clock::now() > hit_cooldown_)
                {
                    LOG_DEBUG("enemy", "hit!");
                    hit_cooldown_ = std::chrono::system_clock::now() + 500ms;
                    const auto shunt_dir =
                        iris::Vector3::normalise(character_controller_->position() - player_->position());
                    character_controller_->shunt(shunt_dir, 6.0, 200ms);

                    health_ -= 25.0f;
                    health_bar_scale_.x = 1.5f * (health_ / 100.0f);
                }
                break;
            }
        }
        default: break;
    }
}

iris::Vector3 Enemy::position() const
{
    return render_entity_->position();
}

}
