////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include "player.h"

#include "iris/core/root.h"
#include "iris/core/transform.h"
#include "iris/graphics/mesh_manager.h"
#include "iris/graphics/scene.h"
#include "iris/physics/physics_system.h"

namespace trinket
{

Player::Player(iris::Scene &scene, iris::PhysicsSystem *ps)
    : render_entity_(nullptr)
    , character_controller_(nullptr)
{
    auto &mesh_manager = iris::Root::mesh_manager();

    render_entity_ = scene.create_entity(
        nullptr, mesh_manager.cube({0.2f, 0.2f, 0.2f}), iris::Transform{{}, {}, {0.5f, 1.7f, 0.5f}});

    character_controller_ = ps->create_character_controller();
    character_controller_->reposition(render_entity_->position(), {});
}

void Player::update()
{
    render_entity_->set_position(character_controller_->position());
}

void Player::set_orientation(const iris::Quaternion &orientation)
{
    render_entity_->set_orientation(orientation);
}

void Player::set_walk_direction(const iris::Vector3 &direction)
{
    character_controller_->set_walk_direction(direction);
}

iris::Vector3 Player::position() const
{
    return character_controller_->position();
}

}
