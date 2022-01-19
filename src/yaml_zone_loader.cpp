////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include "yaml_zone_loader.h"

#include <algorithm>
#include <memory>
#include <string>
#include <vector>

#include "iris/core/quaternion.h"
#include "iris/core/resource_loader.h"
#include "iris/core/root.h"
#include "iris/core/vector3.h"
#include "iris/graphics/mesh_manager.h"
#include "iris/graphics/render_graph/render_graph.h"
#include "iris/graphics/render_graph/texture_node.h"
#include "iris/graphics/skeleton.h"
#include "iris/graphics/vertex_data.h"
#include "iris/log/log.h"
#include "iris/physics/collision_shape.h"
#include "iris/physics/physics_manager.h"
#include "iris/physics/physics_system.h"
#include "yaml-cpp/yaml.h"

namespace
{

iris::Vector3 get_vector3(const YAML::Node &node)
{
    return {node[0].as<float>(), node[1].as<float>(), node[2].as<float>()};
}

iris::Quaternion get_quaternion(const YAML::Node &node)
{
    return {node[0].as<float>(), node[1].as<float>(), node[2].as<float>()};
}

iris::Vector3 bounding_box(const iris::Mesh *mesh, const iris::Vector3 &scale)
{
    const auto min_max_x = std::minmax_element(
        std::cbegin(mesh->vertices()),
        std::cend(mesh->vertices()),
        [](const iris::VertexData &a, const iris::VertexData &b) { return a.position.x < b.position.x; });

    const auto min_max_y = std::minmax_element(
        std::cbegin(mesh->vertices()),
        std::cend(mesh->vertices()),
        [](const iris::VertexData &a, const iris::VertexData &b) { return a.position.y < b.position.y; });

    const auto min_max_z = std::minmax_element(
        std::cbegin(mesh->vertices()),
        std::cend(mesh->vertices()),
        [](const iris::VertexData &a, const iris::VertexData &b) { return a.position.z < b.position.z; });

    const iris::Vector3 min_point{
        min_max_x.first->position.x, min_max_y.first->position.y, min_max_z.first->position.z};
    const iris::Vector3 max_point{
        min_max_x.second->position.x, min_max_y.second->position.y, min_max_z.second->position.z};

    return (max_point - min_point) * 0.5f * scale;
}

}

namespace trinket
{

YamlZoneLoader::YamlZoneLoader(const std::string &zone_file)
    : yaml_file_()
{
    const auto config_file_data = iris::ResourceLoader::instance().load(zone_file);
    std::string config_file_str(reinterpret_cast<const char *>(config_file_data.data()), config_file_data.size());

    yaml_file_ = ::YAML::Load(config_file_str);
}

std::string YamlZoneLoader::name()
{
    return yaml_file_["name"].as<std::string>();
}

iris::Vector3 YamlZoneLoader::player_start_position()
{
    return get_vector3(yaml_file_["player_start_position"]);
}

std::vector<StaticGeometry> YamlZoneLoader::static_geometry()
{
    std::vector<StaticGeometry> static_geometry{};

    auto *ps = iris::Root::physics_manager().current_physics_system();

    for (const auto &geometry : yaml_file_["static_geometry"])
    {
        const auto scale = get_vector3(geometry["scale"]);
        const auto mesh_type = geometry["mesh_type"].as<std::string>();
        const auto *mesh = (mesh_type == "cube") ? iris::Root::mesh_manager().cube({1.0f, 1.0f, 0.0f})
                                                 : iris::Root::mesh_manager().load_mesh(mesh_type);
        const auto texture_name = geometry["texture"].as<std::string>();

        auto render_graph = std::make_unique<iris::RenderGraph>();
        auto *texture_node = render_graph->create<iris::TextureNode>(texture_name);
        render_graph->render_node()->set_colour_input(texture_node);

        iris::CollisionShape *collision_shape = nullptr;
        if (geometry["rigid_body"].as<bool>())
        {
            collision_shape = geometry["rigid_body_type"].as<std::string>() == "bounding_box"
                                  ? ps->create_box_collision_shape(bounding_box(mesh, scale))
                                  : ps->create_mesh_collision_shape(mesh, scale);
        }

        static_geometry.push_back(
            {.position = get_vector3(geometry["position"]),
             .orientation = get_quaternion(geometry["orientation"]),
             .scale = scale,
             .mesh = mesh,
             .render_graph = std::move(render_graph),
             .collision_shape = collision_shape,
             .name = geometry["mesh_type"].as<std::string>()});
    }

    return static_geometry;
}

std::vector<EnemyInfo> YamlZoneLoader::enemies()
{
    std::vector<EnemyInfo> enemies{};

    for (const auto &enemy : yaml_file_["enemies"])
    {
        const auto mesh_name = enemy["mesh"].as<std::string>();
        const auto *mesh = iris::Root::mesh_manager().load_mesh(mesh_name);
        const auto texture_name = enemy["texture"].as<std::string>();

        auto render_graph = std::make_unique<iris::RenderGraph>();
        auto *texture_node = render_graph->create<iris::TextureNode>(texture_name);
        render_graph->render_node()->set_colour_input(texture_node);

        enemies.push_back(
            {.script_file = enemy["script"].as<std::string>(),
             .mesh = mesh,
             .render_graph = std::move(render_graph),
             .skeleton = iris::Root::mesh_manager().load_skeleton(mesh_name),
             .animations = iris::Root::mesh_manager().load_animations(mesh_name),
             .position = get_vector3(enemy["position"]),
             .orientation = get_quaternion(enemy["orientation"]),
             .scale = get_vector3(enemy["scale"])});
    }

    return enemies;
}

std::tuple<iris::Transform, std::string> YamlZoneLoader::portal()
{
    const auto portal = *yaml_file_["portal"].begin();

    return {
        iris::Transform{get_vector3(portal["position"]), {}, get_vector3(portal["scale"])},
        portal["destination"].as<std::string>()};
}

}
