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
#include "iris/physics/physics_system.h"
#include "iris/physics/rigid_body.h"

#include "message_type.h"

namespace trinket
{

Enemy::Enemy(const iris::Vector3 &position, iris::Scene &scene, iris::PhysicsSystem *ps)
    : render_entity_(nullptr)
    , rigid_body_(nullptr)
{
    auto &mesh_manager = iris::Root::mesh_manager();

    render_entity_ =
        scene.create_entity(nullptr, mesh_manager.cube({1.0f, 0.0f, 0.0f}), iris::Transform{position, {}, {2.0f}});
    rigid_body_ = ps->create_rigid_body(
        render_entity_->position(), ps->create_box_collision_shape({2.0f}), iris::RigidBodyType::NORMAL);
}

void Enemy::update()
{
    render_entity_->set_position(rigid_body_->position());
    render_entity_->set_orientation(rigid_body_->orientation());
}

void Enemy::handle_message(MessageType, const std::any &)
{
}

}
