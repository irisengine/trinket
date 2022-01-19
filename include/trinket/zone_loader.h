////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <memory>
#include <string>
#include <tuple>
#include <vector>

#include "iris/core/quaternion.h"
#include "iris/core/transform.h"
#include "iris/core/vector3.h"
#include "iris/graphics/animation/animation.h"
#include "iris/graphics/mesh.h"
#include "iris/graphics/render_graph/render_graph.h"
#include "iris/graphics/skeleton.h"
#include "iris/physics/collision_shape.h"

namespace trinket
{

struct StaticGeometry
{
    iris::Vector3 position;
    iris::Quaternion orientation;
    iris::Vector3 scale;
    const iris::Mesh *mesh;
    std::unique_ptr<iris::RenderGraph> render_graph;
    iris::CollisionShape *collision_shape;
    std::string name;
};

struct EnemyInfo
{
    std::string script_file;
    const iris::Mesh *mesh;
    std::unique_ptr<iris::RenderGraph> render_graph;
    iris::Skeleton skeleton;
    std::vector<iris::Animation> animations;
    iris::Vector3 position;
    iris::Vector3 scale;
    iris::Quaternion orientation;
};

class ZoneLoader
{
  public:
    virtual ~ZoneLoader() = default;
    virtual std::string name() = 0;
    virtual iris::Vector3 player_start_position() = 0;
    virtual std::vector<StaticGeometry> static_geometry() = 0;
    virtual std::vector<EnemyInfo> enemies() = 0;
    virtual std::tuple<iris::Transform, std::string> portal() = 0;
};

}
