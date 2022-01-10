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
#include "iris/core/error_handling.h"
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

Game::Game(std::unique_ptr<Config> config, std::vector<std::unique_ptr<ZoneLoader>> &&zone_loaders)
    : running_(true)
    , config_(std::move(config))
    , zone_loaders_(std::move(zone_loaders))
    , current_zone_(nullptr)
    , next_zone_(nullptr)
    , window_(nullptr)
    , portal_(nullptr)
    , portal_destination_()

{
    const auto starting_zone_name = config_->string_option(ConfigOption::STARTING_ZONE);

    auto starting_zone = std::find_if(
        std::begin(zone_loaders_),
        std::end(zone_loaders_),
        [&starting_zone_name](auto &element) { return element->name() == starting_zone_name; });

    iris::ensure(starting_zone != std::end(zone_loaders_), "missing starting zone");

    next_zone_ = starting_zone->get();

    subscribe(MessageType::QUIT);
    subscribe(MessageType::KEY_PRESS);
}

void Game::run()
{
    window_ = iris::Root::window_manager().create_window(
        config_->uint32_option(ConfigOption::SCREEN_WIDTH), config_->uint32_option(ConfigOption::SCREEN_HEIGHT));

    do
    {
        current_zone_ = next_zone_;
        next_zone_ = nullptr;

        run_zone();
    } while (running_);
}

void Game::run_zone()
{
    auto &mesh_manager = iris::Root::mesh_manager();

    // basic scene setup
    iris::Scene scene{};

    scene.set_ambient_light({0.2f, 0.2f, 0.2f, 1.0f});
    scene.create_light<iris::PointLight>(iris::Vector3{10.0f}, iris::Colour{100.0f, 100.0f, 100.0f});

    const auto sky_box = iris::Root::texture_manager().create(
        iris::Colour{0.275f, 0.51f, 0.796f}, iris::Colour{0.5f, 0.5f, 0.5f}, 2048u, 2048u);

    auto *ps = iris::Root::physics_manager().create_physics_system();

    for (auto &geometry : current_zone_->static_geometry())
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
            body->set_name(geometry.name);
        }
    }

    const auto [portal_transform, destination] = current_zone_->portal();
    scene.create_entity(nullptr, mesh_manager.cube({}), portal_transform);
    portal_ = ps->create_rigid_body(
        portal_transform.translation(),
        ps->create_box_collision_shape(portal_transform.scale()),
        iris::RigidBodyType::GHOST);
    portal_destination_ = destination;

    auto debug_mesh = mesh_manager.unique_cube({});

    if (config_->bool_option(ConfigOption::PHYSICS_DEBUG_DRAW))
    {
        auto *debug_draw = scene.create_entity(nullptr, debug_mesh.get(), iris::Vector3{}, iris::PrimitiveType::LINES);
        ps->enable_debug_draw(debug_draw);
    }

    std::vector<std::unique_ptr<GameObject>> objects{};
    objects.emplace_back(std::make_unique<InputHandler>(window_));
    objects.emplace_back(std::make_unique<Enemy>(iris::Vector3{10.0f, 0.0f, 0.0f}, scene, ps));

    objects.emplace_back(std::make_unique<Player>(scene, ps, current_zone_->player_start_position()));
    auto *player = static_cast<Player *>(objects.back().get());

    objects.emplace_back(std::make_unique<ThirdPersonCamera>(player, window_->width(), window_->height(), ps));
    auto *camera = static_cast<ThirdPersonCamera *>(objects.back().get());

    iris::RenderPass render_pass{&scene, camera->camera(), nullptr, sky_box};
    window_->set_render_passes({render_pass});

    iris::Looper looper{
        0ms,
        16ms,
        [ps, player, this](auto, std::chrono::microseconds delta)
        {
            ps->step(std::chrono::duration_cast<std::chrono::milliseconds>(delta));

            for (const auto &contact : ps->contacts(portal_))
            {
                if (contact.contact_b == player->rigid_body())
                {
                    auto destination = std::find_if(
                        std::begin(zone_loaders_),
                        std::end(zone_loaders_),
                        [this](auto &element) { return element->name() == portal_destination_; });

                    iris::ensure(destination != std::cend(zone_loaders_), "missing zone");

                    next_zone_ = destination->get();
                }
            }

            return true;
        },
        [&](auto, auto)
        {
            for (auto &object : objects)
            {
                object->update();
            }

            window_->render();

            return running_ && (next_zone_ == nullptr);
        }};

    looper.run();
}

void Game::handle_message(MessageType message_type, const std::any &data)
{
    switch (message_type)
    {
        case MessageType::QUIT: running_ = false; break;
        case MessageType::KEY_PRESS:
        {
            const auto key = std::any_cast<iris::KeyboardEvent>(data);
            if ((key.key == iris::Key::R) && (key.state == iris::KeyState::DOWN))
            {
                next_zone_ = current_zone_;
            }
            break;
        }
        default: break;
    }
}

}