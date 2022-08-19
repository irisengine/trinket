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

#include "iris/core/error_handling.h"
#include "iris/core/quaternion.h"
#include "iris/core/resource_loader.h"
#include "iris/core/root.h"
#include "iris/core/vector3.h"
#include "iris/graphics/animation/animation_layer.h"
#include "iris/graphics/mesh_manager.h"
#include "iris/graphics/render_graph/arithmetic_node.h"
#include "iris/graphics/render_graph/render_graph.h"
#include "iris/graphics/render_graph/texture_node.h"
#include "iris/graphics/render_graph/value_node.h"
#include "iris/graphics/render_graph/vertex_node.h"
#include "iris/graphics/scene.h"
#include "iris/graphics/single_entity.h"
#include "iris/graphics/skeleton.h"
#include "iris/graphics/texture_manager.h"
#include "iris/graphics/vertex_data.h"
#include "iris/log/log.h"
#include "iris/physics/collision_shape.h"
#include "iris/physics/physics_manager.h"
#include "iris/physics/physics_system.h"

#include "yaml-cpp/yaml.h"

#include "enemy.h"
#include "game_object.h"
#include "npc.h"
#include "player.h"
#include "third_person_camera.h"

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

using namespace std::literals::chrono_literals;

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

void YamlZoneLoader::load_static_geometry(
    iris::PhysicsSystem *ps,
    iris::Scene *scene,
    iris::RenderPipeline &render_pipeline)
{
    std::unordered_map<const iris::Mesh *, std::vector<iris::Transform>> instances{};
    std::unordered_map<const iris::Mesh *, iris::RenderGraph *> render_graphs{};

    for (const auto &geometry : yaml_file_["static_geometry"])
    {
        const auto position = get_vector3(geometry["position"]);
        const auto orientation = get_quaternion(geometry["orientation"]);
        const auto scale = get_vector3(geometry["scale"]);
        const auto mesh_type = geometry["mesh_type"].as<std::string>();

        std::vector<const iris::Mesh *> meshes{};

        if (mesh_type == "cube")
        {
            meshes.push_back(iris::Root::mesh_manager().cube({}));
        }
        else
        {
            const auto mesh_data = iris::Root::mesh_manager().load_mesh(mesh_type).mesh_data;
            std::transform(
                std::begin(mesh_data), std::end(mesh_data), std::back_inserter(meshes), [](const auto &element) {
                    return element.mesh;
                });
        }

        int tex_count = 0u;

        for (const auto *mesh : meshes)
        {
            instances[mesh].emplace_back(position, orientation, scale);
            render_graphs.try_emplace(mesh, nullptr);

            if (geometry["texture"])
            {
                const auto texture_name = geometry["texture"][tex_count].as<std::string>();
                ++tex_count;

                render_graphs[mesh] = render_pipeline.create_render_graph();
                iris::TextureNode *texture_node = nullptr;

                if (geometry["texture_scale"])
                {
                    auto *rg = render_graphs[mesh];
                    texture_node = rg->create<iris::TextureNode>(
                        texture_name,
                        iris::TextureUsage::IMAGE,
                        nullptr,
                        iris::UVSource::NODE,
                        rg->create<iris::ArithmeticNode>(
                            rg->create<iris::VertexNode>(iris::VertexDataType::UV),
                            rg->create<iris::ValueNode<iris::Vector3>>(
                                iris::Vector3{geometry["texture_scale"].as<float>()}),
                            iris::ArithmeticOperator::MULTIPLY));
                }
                else
                {
                    const auto *sampler = iris::Root::texture_manager().create(iris::SamplerDescriptor{
                        .minification_filter = iris::SamplerFilter::NEAREST,
                        .magnification_filter = iris::SamplerFilter::NEAREST,
                        .uses_mips = false,
                        .mip_filter = iris::SamplerFilter::LINEAR});
                    texture_node = render_graphs[mesh]->create<iris::TextureNode>(
                        texture_name, iris::TextureUsage::IMAGE, sampler);
                }

                render_graphs[mesh]->render_node()->set_colour_input(texture_node);

                if (geometry["normal"])
                {
                    const auto normal_name = geometry["normal"].as<std::string>();
                    auto *normal_node = render_graphs[mesh]->create<iris::TextureNode>(normal_name);
                    render_graphs[mesh]->render_node()->set_normal_input(normal_node);
                }
            }

            if (geometry["rigid_body"].as<bool>())
            {
                const auto *collision_shape = geometry["rigid_body_type"].as<std::string>() == "bounding_box"
                                                  ? ps->create_box_collision_shape(bounding_box(mesh, scale))
                                                  : ps->create_mesh_collision_shape(mesh, scale);
                auto *body = ps->create_rigid_body(position, collision_shape, iris::RigidBodyType::STATIC);
                body->reposition(position, orientation);
                body->set_name(mesh_type);
            }
        }
    }

    for (const auto &[mesh, transforms] : instances)
    {
        if (transforms.size() == 1)
        {
            scene->create_entity<iris::SingleEntity>(render_graphs[mesh], mesh, transforms.front());
        }
        else
        {
            scene->create_entity<iris::InstancedEntity>(render_graphs[mesh], mesh, transforms);
        }
    }
}

void YamlZoneLoader::load_enemies(
    iris::PhysicsSystem *ps,
    iris::Scene *scene,
    iris::RenderPipeline &render_pipeline,
    std::vector<std::unique_ptr<GameObject>> &game_objects,
    Player *player,
    ThirdPersonCamera *camera)
{
    // game_objects.push_back(std::make_unique<NPC>(
    //    scene,
    //    iris::Vector3{30.0f, 0.0f, -10.0f},
    //    render_pipeline,
    //    "Knight_Golden_Female.fbx",
    //    "CharacterArmature|Victory"));

    // game_objects.push_back(std::make_unique<NPC>(
    //    scene, iris::Vector3{20.0f, 0.0f, 30.0f}, render_pipeline, "Wolf.fbx", "AnimalArmature|Idle_2"));

    for (const auto &enemy : yaml_file_["enemies"])
    {
        const auto position = get_vector3(enemy["position"]);
        const auto orientation = get_quaternion(enemy["orientation"]);
        const auto scale = get_vector3(enemy["scale"]);

        const auto bounds_min = get_vector3(enemy["bounds_min"]);
        const auto bounds_max = get_vector3(enemy["bounds_max"]);

        const auto mesh_name = enemy["mesh"].as<std::string>();
        const auto mesh_data = iris::Root::mesh_manager().load_mesh(mesh_name);
        iris::expect(mesh_data.mesh_data.size() == 1u, "expecting only one mesh");
        const auto texture_name = enemy["texture"].as<std::string>();

        auto render_graph = render_pipeline.create_render_graph();
        auto *texture_node = render_graph->create<iris::TextureNode>(texture_name);
        render_graph->render_node()->set_colour_input(texture_node);

        const auto script_file = enemy["script"].as<std::string>();

        auto *health_bar = scene->create_entity<iris::SingleEntity>(
            nullptr,
            iris::Root::mesh_manager().sprite({1.0f, 0.0f, 0.0f}),
            iris::Transform({}, {}, {1.5f, 0.1f, 1.0f}));

        auto *entity = scene->create_entity<iris::SingleEntity>(
            render_graph,
            mesh_data.mesh_data.front().mesh,
            iris::Transform(position, orientation, scale),
            mesh_data.skeleton);
        game_objects.emplace_back(std::make_unique<Enemy>(
            ps, script_file, entity, health_bar, mesh_data.animations, bounds_min, bounds_max, player, camera));
    }
}

std::tuple<iris::Transform, std::string> YamlZoneLoader::portal()
{
    const auto n = yaml_file_["name"].as<std::string>();
    const auto &portal = yaml_file_["portal"];
    const auto p = portal["position"];
    const auto s = portal["scale"];
    const auto d = portal["destination"];

    return {
        iris::Transform{get_vector3(portal["position"]), {}, get_vector3(portal["scale"])},
        portal["destination"].as<std::string>()};
}

}
