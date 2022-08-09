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
#include <unordered_map>
#include <vector>

#include "iris/core/quaternion.h"
#include "iris/core/vector3.h"
#include "iris/graphics/animation/animation_controller.h"
#include "iris/graphics/render_pipeline.h"
#include "iris/graphics/scene.h"
#include "iris/graphics/single_entity.h"
#include "iris/physics/physics_system.h"
#include "iris/physics/rigid_body.h"

#include "character_controller.h"
#include "game_object.h"
#include "message_type.h"
#include "publisher.h"
#include "subscriber.h"

namespace trinket
{

class Player : public GameObject, Publisher, Subscriber
{
  public:
    Player(
        iris::Scene *scene,
        iris::PhysicsSystem *ps,
        const iris::Vector3 &start_position,
        iris::RenderPipeline &render_pipeline);
    ~Player() override = default;

    void update(std::chrono::microseconds) override;

    void set_orientation(const iris::Quaternion &orientation);
    void set_walk_direction(const iris::Vector3 &direction);
    iris::Vector3 position() const;
    const iris::RigidBody *rigid_body() const;

    void handle_message(MessageType message_type, const std::any &data) override;

  private:
    struct SubMesh
    {
        std::string bone_attach;
        iris::Matrix4 transform;
        iris::Vector3 offset;
    };

    std::vector<iris::SingleEntity *> render_entities_;
    CharacterController *character_controller_;
    iris::RigidBody *sword_body_;
    bool attacking_;
    std::chrono::system_clock::time_point attack_stop_;
    std::chrono::milliseconds attack_duration_;
    iris::PhysicsSystem *ps_;
    std::chrono::system_clock::time_point blend_stop_;
    std::chrono::milliseconds blend_time_;
    bool blending_;
    std::unique_ptr<iris::AnimationController> animation_controller_;
    std::uint32_t move_key_pressed_;
    float health_;
    iris::Skeleton *skeleton_;
    std::unordered_map<iris::SingleEntity *, SubMesh> sub_meshes_;
    iris::SingleEntity *sword_entity_;
    std::uint32_t xp_;
    std::uint32_t next_level_;
    bool lock_ = false;
    iris::Vector3 p;
};

}
