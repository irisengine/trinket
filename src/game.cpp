////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include "game.h"

#include <algorithm>
#include <any>
#include <chrono>
#include <cmath>
#include <iostream>
#include <map>
#include <memory>
#include <vector>

#include "iris/core/camera.h"
#include "iris/core/colour.h"
#include "iris/core/looper.h"
#include "iris/core/root.h"
#include "iris/core/transform.h"
#include "iris/events/event.h"
#include "iris/graphics/mesh_manager.h"
#include "iris/graphics/render_entity.h"
#include "iris/graphics/render_graph/render_graph.h"
#include "iris/graphics/render_graph/texture_node.h"
#include "iris/graphics/render_pass.h"
#include "iris/graphics/scene.h"
#include "iris/graphics/texture_manager.h"
#include "iris/graphics/window.h"
#include "iris/graphics/window_manager.h"
#include "iris/physics/basic_character_controller.h"
#include "iris/physics/physics_manager.h"
#include "iris/physics/physics_system.h"
#include "iris/physics/rigid_body.h"
#include "iris/physics/rigid_body_type.h"

#include "config.h"
#include "enemy.h"
#include "game_object.h"
#include "input_handler.h"
#include "maths.h"
#include "message_type.h"
#include "player.h"
#include "publisher.h"
#include "third_person_camera.h"
#include "zone_loader.h"

using namespace std::literals::chrono_literals;

namespace trinket
{

Game::Game(std::unique_ptr<Config> config, std::unique_ptr<ZoneLoader> zone_loader)
    : running_(true)
    , config_(std::move(config))
    , zone_loader_(std::move(zone_loader))
{
    subscribe(MessageType::QUIT);
}

void Game::run()
{
    // window and camera setup - we will use a 3rd person camera for this game
    auto *window = iris::Root::window_manager().create_window(
        config_->uint32_option(ConfigOption::SCREEN_WIDTH), config_->uint32_option(ConfigOption::SCREEN_HEIGHT));

    auto &mesh_manager = iris::Root::mesh_manager();

    // basic scene setup
    iris::Scene scene{};
    auto *box = scene.create_entity(
        nullptr, mesh_manager.cube({}), iris::Transform{{-10.0f, 0.0f, 0.0f}, {}, {0.5f, 1.7f, 0.5f}});

    scene.set_ambient_light({0.2f, 0.2f, 0.2f, 1.0f});
    scene.create_light<iris::PointLight>(iris::Vector3{10.0f}, iris::Colour{100.0f, 100.0f, 100.0f});

    const auto sky_box = iris::Root::texture_manager().create(
        iris::Colour{0.275f, 0.51f, 0.796f}, iris::Colour{0.5f, 0.5f, 0.5f}, 2048u, 2048u);

    auto *ps = iris::Root::physics_manager().current_physics_system();

    // auto *render_graph = scene.create_render_graph();
    // auto *texture = render_graph->create<iris::TextureNode>("Inn_Texture.png");
    // render_graph->render_node()->set_colour_input(texture);
    // scene.create_entity(
    //    render_graph,
    //    mesh_manager.load_mesh("Inn.fbx"),
    //    iris::Transform{{0.0f, -1.0f, 20.0f}, {{1.0f, 0.0f, 0.0f}, -pi_2}, {8.0f}});
    for (auto &geometry : zone_loader_->static_geometry())
    {
        scene.create_entity(
            scene.add(std::move(geometry.render_graph)),
            geometry.mesh,
            iris::Transform{geometry.position, geometry.orientation, geometry.scale});
        if (geometry.collision_shape != nullptr)
        {
            auto *body =
                ps->create_rigid_body(geometry.position, geometry.collision_shape, iris::RigidBodyType::STATIC);
            body->reposition(geometry.position, geometry.orientation);
        }
    }

    auto debug_mesh = mesh_manager.unique_cube({});
    auto *debug_draw = scene.create_entity(nullptr, debug_mesh.get(), iris::Vector3{}, iris::PrimitiveType::LINES);
    ps->enable_debug_draw(debug_draw);

    std::vector<std::unique_ptr<GameObject>> objects{};
    objects.emplace_back(std::make_unique<InputHandler>(window));
    objects.emplace_back(std::make_unique<Enemy>(iris::Vector3{10.0f, 0.0f, 0.0f}, scene, ps));

    objects.emplace_back(std::make_unique<Player>(scene, ps));
    auto *player = static_cast<Player *>(objects.back().get());

    objects.emplace_back(std::make_unique<ThirdPersonCamera>(player, window->width(), window->height()));
    auto *camera = static_cast<ThirdPersonCamera *>(objects.back().get());

    ps->create_rigid_body(
        box->position(), ps->create_box_collision_shape({0.5f, 1.7f, 0.5f}), iris::RigidBodyType::STATIC);

    iris::RenderPass render_pass{&scene, camera->camera(), nullptr, sky_box};
    window->set_render_passes({render_pass});

    iris::Looper looper{
        0ms,
        16ms,
        [ps](auto, std::chrono::microseconds delta)
        {
            ps->step(std::chrono::duration_cast<std::chrono::milliseconds>(delta));
            return true;
        },
        [&](auto, auto)
        {
            for (auto &object : objects)
            {
                object->update();
            }

            window->render();

            return running_;
        }};

    looper.run();
}

void Game::handle_message(MessageType message_type, const std::any &data)
{
    switch (message_type)
    {
        case MessageType::QUIT: running_ = false; break;
        default: break;
    }
}

}