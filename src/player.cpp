
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
#include "iris/graphics/bone.h"
#include "iris/graphics/mesh_manager.h"
#include "iris/graphics/render_graph/render_graph.h"
#include "iris/graphics/render_graph/texture_node.h"
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
    , attacking_(false)
    , attack_stop_()
    , attack_duration_(800ms)
    , ps_(ps)
    , current_animation_("CharacterArmature|Idle_Weapon")
{
    auto &mesh_manager = iris::Root::mesh_manager();

    auto *render_graph = scene.create_render_graph();
    auto *texture = render_graph->create<iris::TextureNode>("Warrior_Texture.png");
    render_graph->render_node()->set_colour_input(texture);

    render_entity_ = scene.create_entity(
        render_graph,
        mesh_manager.load_mesh("Warrior.fbx"),
        iris::Transform{{}, {}, {0.01f}},
        mesh_manager.load_skeleton("Warrior.fbx"));
    render_entity_->skeleton().set_animation(current_animation_);

    auto *render_graph2 = scene.create_render_graph();
    auto *texture2 = render_graph2->create<iris::TextureNode>("Sword_Texture.png");
    render_graph2->render_node()->set_colour_input(texture2);
    sword_ = scene.create_entity(
        render_graph2, mesh_manager.load_mesh("Sword.fbx"), iris::Transform{{}, {}, {1.0f, 0.1f, 0.1f}});

    sword_body_ =
        ps->create_rigid_body({}, ps->create_box_collision_shape({0.1f, 0.1f, 1.0f}), iris::RigidBodyType::GHOST);
    sword_body_->set_name("sword");

    character_controller_ = ps->create_character_controller();
    character_controller_->reposition(render_entity_->position(), {});

    subscribe(MessageType::KEY_PRESS);
}

void Player::update()
{
    // handle attack logic if player is attacking
    if (attacking_)
    {
        const auto now = std::chrono::system_clock::now();
        if (now >= attack_stop_)
        {
            // attack has completed so reset state
            attacking_ = false;
            render_entity_->skeleton().set_animation("CharacterArmature|Idle_Weapon");
        }

        // get all contact points the sword is making (ignoring the player) and publish a message
        for (auto &contact : ps_->contacts(sword_body_))
        {
            if (contact.contact_b != character_controller_->rigid_body())
            {
                publish(MessageType::WEAPON_COLLISION, {std::make_tuple(contact.contact_b, contact.world_position_b)});
            }
        }
    }

    render_entity_->set_position(character_controller_->position());
    render_entity_->skeleton().advance();

    // offset of player in world space
    static constexpr iris::Vector3 player_world_offset{0.0f, -1.2f, 0.0f};
    static constexpr auto player_world_offset_transform = iris::Matrix4::make_translate(player_world_offset);

    // local transform of sword
    const auto sword_rot =
        iris::Matrix4(iris::Quaternion{{0.0f, 1.0f, 0.0f}, -pi_2} * iris::Quaternion{{0.0f, 0.0f, 1.0f}, pi_2});
    const auto sword_translate = iris::Matrix4::make_translate({0.0f, 0.0f, 0.0f});
    const auto sword_scale = iris::Matrix4::make_scale({1.0f});
    const auto sword_transform = sword_rot * sword_translate * sword_scale;

    // bone to attach sword to
    const auto bone_index = render_entity_->skeleton().bone_index("Weapon.R");
    const auto &bone = render_entity_->skeleton().bone(bone_index);
    const auto bone_transform = render_entity_->skeleton().transform(bone_index);

    static constexpr iris::Vector3 sword_body_offset{-1.0f, 0.0f, 0.0f};

    // get bone in transform in world space
    const auto bone_to_world_space = player_world_offset_transform * render_entity_->transform() * bone_transform *
                                     iris::Matrix4::invert(bone.offset());
    const iris::Transform sword_body_transform{
        bone_to_world_space * iris::Matrix4::make_translate(sword_body_offset) * sword_transform};

    // compound transform for sword
    sword_->set_transform(bone_to_world_space * sword_transform);
    sword_body_->reposition(sword_body_transform.translation(), sword_body_transform.rotation());

    render_entity_->set_position(render_entity_->position() + player_world_offset);
}

void Player::set_orientation(const iris::Quaternion &orientation)
{
    render_entity_->set_orientation(orientation * iris::Quaternion{{0.0f, 1.0f, 0.0f}, -M_PI_2});
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
            if (!attacking_)
            {
                // player can only attack if not attacking
                if ((key.key == iris::Key::SPACE) && (key.state == iris::KeyState::DOWN))
                {
                    attacking_ = true;
                    attack_stop_ = std::chrono::system_clock::now() + attack_duration_;
                    current_animation_ = "CharacterArmature|Sword_Attack";
                    render_entity_->skeleton().set_animation(current_animation_);
                }

                if ((key.key == iris::Key::W) || (key.key == iris::Key::A) || (key.key == iris::Key::S) ||
                    (key.key == iris::Key::D))
                {
                    const auto next_animation =
                        key.state == iris::KeyState::DOWN ? "CharacterArmature|Run" : "CharacterArmature|Idle_Weapon";
                    if (next_animation != current_animation_)
                    {
                        current_animation_ = next_animation;
                        render_entity_->skeleton().set_animation(current_animation_);
                    }
                }
            }

            break;
        }
        default: break;
    }
}

}
