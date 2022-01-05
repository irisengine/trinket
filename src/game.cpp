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
#include "iris/core/resource_loader.h"
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

using namespace std::literals::chrono_literals;

namespace trinket
{

Game::Game(std::unique_ptr<Config> config)
    : running_(true)
    , config_(std::move(config))
{
    subscribe(MessageType::QUIT);
}

void Game::run()
{
    if (const auto &graphics_api = config_->string_option(ConfigOption::GRAPHICS_API); graphics_api != "default")
    {
        iris::Root::set_graphics_api(graphics_api);
    }

    // window and camera setup - we will use a 3rd person camera for this game
    auto *window = iris::Root::window_manager().create_window(
        config_->uint32_option(ConfigOption::SCREEN_WIDTH), config_->uint32_option(ConfigOption::SCREEN_HEIGHT));

    auto &mesh_manager = iris::Root::mesh_manager();

    // basic scene setup
    iris::Scene scene{};
    auto *box = scene.create_entity(
        nullptr, mesh_manager.cube({}), iris::Transform{{-10.0f, 0.0f, 0.0f}, {}, {0.5f, 1.7f, 0.5f}});
    auto *ground = scene.create_entity(
        nullptr, mesh_manager.cube({0.0f, 1.0f, 0.0f}), iris::Transform{{0.0f, -1002.0f, 0.0f}, {}, {1000.0f}});
    scene.set_ambient_light({0.2f, 0.2f, 0.2f, 1.0f});
    scene.create_light<iris::PointLight>(iris::Vector3{10.0f}, iris::Colour{100.0f, 100.0f, 100.0f});

    const auto sky_box = iris::Root::texture_manager().create(
        iris::Colour{0.275f, 0.51f, 0.796f}, iris::Colour{0.5f, 0.5f, 0.5f}, 2048u, 2048u);

    auto *ps = iris::Root::physics_manager().create_physics_system();

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

    ps->create_rigid_body(ground->position(), ps->create_box_collision_shape({1000.0f}), iris::RigidBodyType::STATIC);
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