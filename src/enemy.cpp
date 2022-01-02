////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include "enemy.h"

#include <any>

#include "iris/core/root.h"
#include "iris/core/vector3.h"
#include "iris/graphics/mesh_manager.h"
#include "iris/graphics/render_entity.h"
#include "iris/graphics/scene.h"
#include "iris/log/log.h"
#include "iris/physics/physics_system.h"
#include "iris/physics/rigid_body.h"

#include "message_type.h"

using namespace std::literals::chrono_literals;

namespace trinket
{

Enemy::Enemy(const iris::Vector3 &position, iris::Scene &scene, iris::PhysicsSystem *ps)
    : render_entity_(nullptr)
    , rigid_body_(nullptr)
    , hit_cooldown_()
{
    auto &mesh_manager = iris::Root::mesh_manager();

    render_entity_ =
        scene.create_entity(nullptr, mesh_manager.cube({1.0f, 0.0f, 0.0f}), iris::Transform{position, {}, {2.0f}});
    rigid_body_ = ps->create_rigid_body(
        render_entity_->position(), ps->create_box_collision_shape({2.0f}), iris::RigidBodyType::NORMAL);

    subscribe(MessageType::WEAPON_COLLISION);
}

void Enemy::update()
{
    render_entity_->set_position(rigid_body_->position());
    render_entity_->set_orientation(rigid_body_->orientation());
}

void Enemy::handle_message(MessageType message_type, const std::any &data)
{
    switch (message_type)
    {
        case MessageType::WEAPON_COLLISION:
        {
            const auto &[body, pos] = std::any_cast<std::tuple<iris::RigidBody *, iris::Vector3>>(data);

            if (body == rigid_body_)
            {
                const auto now = std::chrono::system_clock::now();
                if (now >= hit_cooldown_)
                {
                    auto impulse = (rigid_body_->position() - pos).normalise() * iris::Vector3{50.0f};
                    impulse.y = 0.0f;
                    rigid_body_->apply_impulse(impulse);

                    hit_cooldown_ = now + 500ms;
                }

                LOG_DEBUG("enemy", "i've been hit!");
            }
            break;
        }
        default: break;
    }
}

}
