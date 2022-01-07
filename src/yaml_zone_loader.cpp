////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include "yaml_zone_loader.h"

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

}

namespace trinket
{

YamlZoneLoader::YamlZoneLoader(const std::string &zone_file)
    : zone_file_(zone_file)
{
}

std::vector<StaticGeometry> YamlZoneLoader::static_geometry()
{
    std::vector<StaticGeometry> static_geometry{};

    const auto config_file_data = iris::ResourceLoader::instance().load(zone_file_);
    std::string config_file_str(reinterpret_cast<const char *>(config_file_data.data()), config_file_data.size());

    const auto yaml_zone = ::YAML::Load(config_file_str);

    auto *ps = iris::Root::physics_manager().current_physics_system();

    for (const auto &geometry : yaml_zone["static_geometry"])
    {
        const auto scale = get_vector3(geometry["scale"]);
        const auto mesh_type = geometry["mesh_type"].as<std::string>();
        const auto *mesh = (mesh_type == "cube") ? iris::Root::mesh_manager().cube({1.0f, 1.0f, 0.0f})
                                                 : iris::Root::mesh_manager().load_mesh(mesh_type);
        const auto texture_name = geometry["texture"].as<std::string>();

        auto render_graph = std::make_unique<iris::RenderGraph>();
        auto *texture_node = render_graph->create<iris::TextureNode>(texture_name);
        render_graph->render_node()->set_colour_input(texture_node);

        static_geometry.push_back(
            {.position = get_vector3(geometry["position"]),
             .orientation = get_quaternion(geometry["orientation"]),
             .scale = scale,
             .mesh = mesh,
             .render_graph = std::move(render_graph),
             .collision_shape = ps->create_mesh_collision_shape(mesh, scale)});
    }

    return static_geometry;
}

}
