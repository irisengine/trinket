////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include "npc.h"

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

NPC::NPC(
    iris::Scene *scene,
    const iris::Vector3 &start_position,
    iris::RenderPipeline &render_pipeline,
    const std::string &mesh_file,
    const std::string &animation)
    : render_entities_()
    , animation_controller_()
    , skeleton_(nullptr)
    , sub_meshes_()
{
    auto &mesh_manager = iris::Root::mesh_manager();

    auto meshes = mesh_manager.load_mesh(mesh_file);
    auto animations = std::move(meshes.animations);
    skeleton_ = meshes.skeleton;

    const auto scale = 0.01f;

    for (const auto &mesh : meshes.mesh_data)
    {
        render_entities_.emplace_back(scene->create_entity<iris::SingleEntity>(
            nullptr, mesh.mesh, iris::Transform{start_position, {}, {scale}}, skeleton_));
    }

    //    auto *left_shoulder = render_entities_.emplace_back(scene->create_entity<iris::SingleEntity>(
    //        render_graph, meshes.mesh_data[2].mesh, iris::Transform{{}, {}, {0.1f}}, skeleton_));
    //
    //    sub_meshes_[left_shoulder] = SubMesh{
    //        .bone_attach = "UpperArm.R",
    //        .transform = iris::Matrix4(iris::Quaternion{{0.0f, 0.0f, 1.0f}, -pi_2}) *
    //                     iris::Matrix4::make_translate({0.0f, 0.0f, 0.1f}),
    //        .offset = {0.0f, 0.0f, 0.0f}};
    //
    //    auto *right_shoulder = render_entities_.emplace_back(scene->create_entity<iris::SingleEntity>(
    //        render_graph, meshes.mesh_data[3].mesh, iris::Transform{{}, {}, {0.1f}}, skeleton_));
    //
    //    sub_meshes_[right_shoulder] = SubMesh{
    //        .bone_attach = "UpperArm.L",
    //        .transform = iris::Matrix4(iris::Quaternion{{0.0f, 0.0f, 1.0f}, pi_2}) *
    //                     iris::Matrix4::make_translate({0.0f, 0.0f, 0.1f}),
    //        .offset = {0.0f, 0.0f, 0.0f}};
    //
    animation_controller_ = std::make_unique<iris::AnimationController>(
        animations,
        std::vector<iris::AnimationLayer>{
            {{
                 {animation, animation, 0ms},
             },
             animation}},
        skeleton_);
}

void NPC::update(std::chrono::microseconds)
{
    animation_controller_->update();

    // offset of player in world space
    // static constexpr iris::Vector3 player_world_offset{0.0f, -2.0f, 0.0f};
    // static constexpr auto player_world_offset_transform = iris::Matrix4::make_translate(player_world_offset);

    // for (auto &[entity, sub_mesh] : sub_meshes_)
    //{
    //    // bone to attach sword to
    //    const auto bone_index = skeleton_->bone_index(sub_mesh.bone_attach);
    //    const auto bone_transform = skeleton_->transform(bone_index);
    //    const auto &bone = skeleton_->bone(bone_index);

    //    // get bone transform in world space
    //    const auto bone_to_world_space = render_entities_.front()->transform() * bone_transform *
    //                                     iris::Matrix4::invert(bone.offset()) * sub_mesh.transform;

    //    // compound transform for sword::
    //    entity->set_transform(bone_to_world_space);
    //}
}

}