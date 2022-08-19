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

class NPC : public GameObject
{
  public:
    NPC(iris::Scene *scene,
        const iris::Vector3 &start_position,
        iris::RenderPipeline &render_pipeline,
        const std::string &mesh_file,
        const std::string &animation);
    ~NPC() override = default;

    void update(std::chrono::microseconds) override;

  private:
    struct SubMesh
    {
        std::string bone_attach;
        iris::Matrix4 transform;
        iris::Vector3 offset;
    };

    std::vector<iris::SingleEntity *> render_entities_;
    std::unique_ptr<iris::AnimationController> animation_controller_;
    iris::Skeleton *skeleton_;
    std::unordered_map<iris::SingleEntity *, SubMesh> sub_meshes_;
};

}
