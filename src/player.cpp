////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include "player.h"

#include <any>
#include <chrono>
#include <tuple>

#include "iris/core/error_handling.h"
#include "iris/core/matrix4.h"
#include "iris/core/root.h"
#include "iris/core/transform.h"
#include "iris/events/keyboard_event.h"
#include "iris/events/mouse_button_event.h"
#include "iris/graphics/bone.h"
#include "iris/graphics/mesh_manager.h"
#include "iris/graphics/render_graph/arithmetic_node.h"
#include "iris/graphics/render_graph/render_graph.h"
#include "iris/graphics/render_graph/texture_node.h"
#include "iris/graphics/render_graph/value_node.h"
#include "iris/graphics/render_pipeline.h"
#include "iris/graphics/scene.h"
#include "iris/log/log.h"
#include "iris/physics/contact_point.h"
#include "iris/physics/physics_system.h"
#include "iris/physics/rigid_body.h"
#include "iris/physics/rigid_body_type.h"

#include "character_controller.h"
#include "maths.h"
#include "message_type.h"

using namespace std::literals::chrono_literals;

namespace trinket
{

Player::Player(
    iris::Scene *scene,
    iris::PhysicsSystem *ps,
    const iris::Vector3 &start_position,
    iris::RenderPipeline &render_pipeline)
    : render_entities_()
    , character_controller_(nullptr)
    , sword_body_(nullptr)
    , attacking_(false)
    , attack_stop_()
    , attack_duration_(800ms)
    , ps_(ps)
    , blend_stop_()
    , blend_time_(500ms)
    , blending_(false)
    , animation_controller_()
    , move_key_pressed_(0u)
    , health_(100.0f)
    , skeleton_(nullptr)
    , sub_meshes_()
    , sword_entity_(nullptr)
    , xp_(0u)
    , next_level_(100u)
{
    auto &mesh_manager = iris::Root::mesh_manager();

    auto meshes = mesh_manager.load_mesh("Warrior.fbx");
    auto animations = std::move(meshes.animations);
    skeleton_ = meshes.skeleton;

    auto *render_graph = render_pipeline.create_render_graph();
    auto *texture = render_graph->create<iris::TextureNode>("Warrior_Texture.png");
    render_graph->render_node()->set_colour_input(texture);

    render_entities_.emplace_back(scene->create_entity<iris::SingleEntity>(
        render_graph, meshes.mesh_data.front().mesh, iris::Transform{start_position, {}, {0.01f}}, skeleton_));

    // load submeshes based on known offsets into mesh data

    auto *hair = render_entities_.emplace_back(scene->create_entity<iris::SingleEntity>(
        render_graph, meshes.mesh_data[4].mesh, iris::Transform{{}, {}, {1.01f}}, skeleton_));

    sub_meshes_[hair] = SubMesh{
        .bone_attach = "Neck",
        .transform = iris::Matrix4::make_translate({0.0f, -2.0f, 0.0f}) *
                     iris::Matrix4(iris::Quaternion{{1.0f, 0.0f, 0.0f}, -pi_2}),
        .offset = {0.0f, 0.0f, 0.0f}};

    auto *left_shoulder = render_entities_.emplace_back(scene->create_entity<iris::SingleEntity>(
        render_graph, meshes.mesh_data[2].mesh, iris::Transform{{}, {}, {10.01f}}, skeleton_));

    sub_meshes_[left_shoulder] = SubMesh{
        .bone_attach = "UpperArm.R",
        .transform = iris::Matrix4(iris::Quaternion{{0.0f, 0.0f, 1.0f}, -pi_2}) *
                     iris::Matrix4::make_translate({0.0f, 0.0f, 0.1f}),
        .offset = {0.0f, 0.0f, 0.0f}};

    auto *right_shoulder = render_entities_.emplace_back(scene->create_entity<iris::SingleEntity>(
        render_graph, meshes.mesh_data[3].mesh, iris::Transform{{}, {}, {10.01f}}, skeleton_));

    sub_meshes_[right_shoulder] = SubMesh{
        .bone_attach = "UpperArm.L",
        .transform = iris::Matrix4(iris::Quaternion{{0.0f, 0.0f, 1.0f}, pi_2}) *
                     iris::Matrix4::make_translate({0.0f, 0.0f, 0.1f}),
        .offset = {0.0f, 0.0f, 0.0f}};

    // set sword attack animation to not loop
    auto sword_attack_animation =
        std::find_if(std::begin(animations), std::end(animations), [](const iris::Animation &animation) {
            return animation.name() == "CharacterArmature|Sword_AttackFast";
        });
    sword_attack_animation->set_playback_type(iris::PlaybackType::SINGLE);

    // create animation controller with required transitions
    animation_controller_ = std::make_unique<iris::AnimationController>(
        animations,
        std::vector<iris::AnimationLayer>{
            {{{"CharacterArmature|Idle_Weapon", "CharacterArmature|Run", 0ms},
              {"CharacterArmature|Idle_Weapon", "CharacterArmature|Idle_Weapon", 0ms},
              {"CharacterArmature|Run", "CharacterArmature|Idle_Weapon", 0ms},
              {"CharacterArmature|Run", "CharacterArmature|Run", 0ms}},
             "CharacterArmature|Idle_Weapon"},
            {{"Shoulder.R",
              "UpperArm.R",
              "ShoulderPad.R",
              "LowerArm.R",
              "Fist.R",
              "Fist1.R",
              "Fist2.R",
              "Fist2.R_end",
              "Weapon.R",
              "Warrior_Sword",
              "Weapon.R_end",
              "Thumb1.R",
              "Thumb2.R",
              "Thumb2.R_end"},
             {
                 {"CharacterArmature|Sword_AttackFast", "CharacterArmature|Sword_AttackFast", 0ms},
             },
             "CharacterArmature|Sword_AttackFast"}},
        skeleton_);

    const auto sword_meshes = mesh_manager.load_mesh("Sword.fbx");
    iris::expect(sword_meshes.mesh_data.size() == 1u, "expecting only one mesh");

    // load sword and attach to hand

    auto *render_graph2 = render_pipeline.create_render_graph();
    render_graph2->render_node()->set_colour_input(render_graph2->create<iris::ArithmeticNode>(
        render_graph2->create<iris::TextureNode>("Sword_Texture.png"),
        render_graph2->create<iris::ValueNode<iris::Colour>>(iris::Colour{1.0f, 1.0f, 1.0f, 1.0f}),
        iris::ArithmeticOperator::MULTIPLY));

    sword_entity_ = scene->create_entity<iris::SingleEntity>(
        render_graph2, sword_meshes.mesh_data.front().mesh, iris::Transform{{}, {}, {1.0f, 0.1f, 0.1f}});

    sub_meshes_[sword_entity_] = SubMesh{
        .bone_attach = "Weapon.R",
        .transform =
            iris::Matrix4(iris::Quaternion{{0.0f, 1.0f, 0.0f}, -pi_2} * iris::Quaternion{{0.0f, 0.0f, 1.0f}, pi_2}),
        .offset = {0.0f, 0.07f, 0.0f}};

    sword_body_ =
        ps->create_rigid_body({}, ps->create_box_collision_shape({0.1f, 0.1f, 1.0f}), iris::RigidBodyType::GHOST);
    sword_body_->set_name("sword");

    character_controller_ = ps->create_character_controller<CharacterController>(ps, 12.0f, 0.5f, 1.7f, 2.0f);
    character_controller_->reposition(render_entities_.front()->position(), {});

    subscribe(MessageType::MOUSE_BUTTON_PRESS);
    subscribe(MessageType::KEY_PRESS);
    subscribe(MessageType::ENEMY_ATTACK);
    subscribe(MessageType::KILLED_ENEMY);
    subscribe(MessageType::QUEST_COMPLETE);
}

void Player::update(std::chrono::microseconds)
{
    const auto now = std::chrono::system_clock::now();

    // handle attack logic if player is attacking
    if (attacking_)
    {
        if (now >= attack_stop_)
        {
            // attack has completed so reset state
            attacking_ = false;
        }

        // get all contact points the sword is making (ignoring the player) and publish a message
        for (auto &contact : ps_->contacts(sword_body_))
        {
            if (contact.contact != character_controller_->rigid_body())
            {
                publish(MessageType::WEAPON_COLLISION, std::make_tuple(contact.contact, contact.position));
            }
        }
    }

    animation_controller_->update();

    // offset of player in world space
    static constexpr iris::Vector3 player_world_offset{0.0f, -2.0f, 0.0f};
    static constexpr auto player_world_offset_transform = iris::Matrix4::make_translate(player_world_offset);

    for (auto &[entity, sub_mesh] : sub_meshes_)
    {
        // bone to attach sword to
        const auto bone_index = skeleton_->bone_index(sub_mesh.bone_attach);
        const auto bone_transform = skeleton_->transform(bone_index);
        const auto &bone = skeleton_->bone(bone_index);

        // get bone transform in world space
        const auto bone_to_world_space = render_entities_.front()->transform() * bone_transform *
                                         iris::Matrix4::invert(bone.offset()) * sub_mesh.transform;

        // compound transform for sword::
        entity->set_transform(bone_to_world_space);

        if (entity == sword_entity_)
        {
            const iris::Transform sword_body_transform{
                bone_to_world_space * iris::Matrix4::make_translate(sub_mesh.offset + iris::Vector3{0.0f, 0.0f, 1.0f})};
            const iris::Transform sword_body_transform2{
                bone_to_world_space * iris::Matrix4::make_translate(sub_mesh.offset)};
            sword_body_->reposition(sword_body_transform.translation(), sword_body_transform.rotation());
            entity->set_position(sword_body_transform2.translation());
            entity->set_orientation(sword_body_transform2.rotation());
        }
    }

    for (const auto &contact : ps_->contacts(character_controller_->rigid_body()))
    {
        if (contact.contact != sword_body_)
        {
            publish(MessageType::OBJECT_COLLISION, std::make_tuple(contact.contact, contact.position));
        }
    }

    if (health_ <= 0.0f)
    {
        publish(MessageType::PLAYER_DIED, {});
        health_ = 1.0f;
    }
}

void Player::set_orientation(const iris::Quaternion &orientation)
{
    render_entities_.front()->set_orientation(orientation * iris::Quaternion{{0.0f, 1.0f, 0.0f}, -M_PI_2});
}

void Player::set_walk_direction(const iris::Vector3 &direction)
{
    character_controller_->set_movement_direction(direction);
}

iris::Vector3 Player::position() const
{
    return character_controller_->position();
}

const iris::RigidBody *Player::rigid_body() const
{
    return character_controller_->rigid_body();
}

void Player::handle_message(MessageType message_type, const std::any &data)
{
    switch (message_type)
    {
        case MessageType::MOUSE_BUTTON_PRESS:
        {
            const auto mouse_button = std::any_cast<iris::MouseButtonEvent>(data);
            if (!attacking_)
            {
                // player can only attack if not attacking
                if ((mouse_button.button == iris::MouseButton::LEFT) &&
                    (mouse_button.state == iris::MouseButtonState::DOWN))
                {
                    attacking_ = true;
                    attack_stop_ = std::chrono::system_clock::now() + attack_duration_;

                    animation_controller_->play(1u, "CharacterArmature|Sword_AttackFast");
                }
            }

            break;
        }
        case MessageType::KEY_PRESS:
        {
            const auto key = std::any_cast<iris::KeyboardEvent>(data);

            // update player animation to running
            if ((key.key == iris::Key::W) || (key.key == iris::Key::A) || (key.key == iris::Key::S) ||
                (key.key == iris::Key::D))
            {
                blend_stop_ = std::chrono::system_clock::now() + blend_time_;
                blending_ = true;
                if (key.state == iris::KeyState::DOWN)
                {
                    animation_controller_->play(0u, "CharacterArmature|Run");
                    ++move_key_pressed_;
                }
                else
                {
                    if (move_key_pressed_ != 0u)
                    {
                        --move_key_pressed_;
                        if (move_key_pressed_ == 0u)
                        {
                            animation_controller_->play(0u, "CharacterArmature|Idle_Weapon");
                        }
                    }
                }
            }

            break;
        }
        case MessageType::ENEMY_ATTACK:
        {
            health_ -= 10.0f;
            publish(MessageType::PLAYER_HEALTH_CHANGE, health_);

            break;
        }
        case MessageType::KILLED_ENEMY:
        {
            xp_ += 30u;
            if (xp_ >= next_level_)
            {
                xp_ %= next_level_;
            }

            publish(MessageType::LEVEL_PROGRESS, static_cast<float>(xp_) / static_cast<float>(next_level_));

            break;
        }
        case MessageType::QUEST_COMPLETE:
        {
            xp_ += std::any_cast<std::uint32_t>(data);
            if (xp_ >= next_level_)
            {
                xp_ %= next_level_;
            }

            publish(MessageType::LEVEL_PROGRESS, static_cast<float>(xp_) / static_cast<float>(next_level_));

            break;
        }
        default: break;
    }
}
}
