
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include "player.h"

#include <any>
#include <chrono>
#include <tuple>

#include "iris/core/matrix4.h"
#include "iris/core/root.h"
#include "iris/core/transform.h"
#include "iris/events/keyboard_event.h"
#include "iris/graphics/mesh_manager.h"
#include "iris/graphics/scene.h"
#include "iris/log/log.h"
#include "iris/physics/contact_point.h"
#include "iris/physics/physics_system.h"
#include "iris/physics/rigid_body.h"
#include "iris/physics/rigid_body_type.h"

#include "maths.h"
#include "message_type.h"

using namespace std::literals::chrono_literals;

namespace trinket
{

Player::Player(iris::Scene &scene, iris::PhysicsSystem *ps)
    : render_entity_(nullptr)
    , character_controller_(nullptr)
    , sword_(nullptr)
    , sword_body_(nullptr)
    , sword_angle_(pi_2)
    , attacking_(false)
    , attack_stop_()
    , attack_duration_(300ms)
    , ps_(ps)
{
    auto &mesh_manager = iris::Root::mesh_manager();

    render_entity_ = scene.create_entity(
        nullptr, mesh_manager.cube({0.2f, 0.2f, 0.2f}), iris::Transform{{}, {}, {0.5f, 1.7f, 0.5f}});

    sword_ = scene.create_entity(
        nullptr, mesh_manager.cube({0.0f, 0.5f, 1.0f}), iris::Transform{{}, {}, {1.0f, 0.1f, 0.1f}});

    sword_body_ =
        ps->create_rigid_body({}, ps->create_box_collision_shape({1.0f, 0.1f, 0.1f}), iris::RigidBodyType::GHOST);
    sword_body_->set_name("sword");

    character_controller_ = ps->create_character_controller();
    character_controller_->reposition(render_entity_->position(), {});

    subscribe(MessageType::KEY_PRESS);
}

void Player::update()
{
    if (attacking_)
    {
        const auto now = std::chrono::system_clock::now();
        if (now >= attack_stop_)
        {
            attacking_ = false;
            sword_angle_ = pi_2;
        }
        else
        {
            const auto remaining = std::chrono::duration_cast<std::chrono::milliseconds>(attack_stop_ - now);
            const auto remaining_percentage =
                static_cast<float>(remaining.count()) / static_cast<float>(attack_duration_.count());

            sword_angle_ = -((pi * remaining_percentage) + pi_2);
        }

        for (auto &contact : ps_->contacts(sword_body_))
        {
            if (contact.contact_b != character_controller_->rigid_body())
            {
                publish(MessageType::WEAPON_COLLISION, {std::make_tuple(contact.contact_b, contact.world_position_b)});
            }
        }
    }

    render_entity_->set_position(character_controller_->position());

    const auto sword_rot = iris::Matrix4(iris::Quaternion{{0.0f, 1.0f, 0.0f}, sword_angle_});
    const auto sword_translate = iris::Matrix4::make_translate({1.0f, 0.0f, 0.0f});
    const auto sword_scale = iris::Matrix4::make_scale({1.0f, 0.1f, 0.1f});
    const auto sword_transform = sword_rot * sword_translate * sword_scale;

    const auto player_transform =
        iris::Matrix4::make_translate(render_entity_->position()) * iris::Matrix4{render_entity_->orientation()};

    sword_->set_transform(player_transform * sword_transform);
    sword_body_->reposition(sword_->position(), sword_->orientation());
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

void Player::handle_message(MessageType message_type, const std::any &data)
{
    switch (message_type)
    {
        case MessageType::KEY_PRESS:
        {
            const auto key = std::any_cast<iris::KeyboardEvent>(data);
            if ((key.key == iris::Key::SPACE) && (key.state == iris::KeyState::DOWN) && !attacking_)
            {
                attacking_ = true;
                attack_stop_ = std::chrono::system_clock::now() + attack_duration_;
            }

            break;
        }
        default: break;
    }
}

}
