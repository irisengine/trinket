////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include <algorithm>
#include <chrono>
#include <cmath>
#include <iostream>
#include <map>

#include "iris/core/camera.h"
#include "iris/core/colour.h"
#include "iris/core/looper.h"
#include "iris/core/root.h"
#include "iris/core/start.h"
#include "iris/core/transform.h"
#include "iris/events/event.h"
#include "iris/graphics/mesh_manager.h"
#include "iris/graphics/render_entity.h"
#include "iris/graphics/render_pass.h"
#include "iris/graphics/scene.h"
#include "iris/graphics/texture_manager.h"
#include "iris/graphics/window.h"
#include "iris/graphics/window_manager.h"
#include "iris/iris_version.h"
#include "iris/physics/basic_character_controller.h"
#include "iris/physics/physics_manager.h"
#include "iris/physics/physics_system.h"
#include "iris/physics/rigid_body.h"
#include "iris/physics/rigid_body_type.h"

static constexpr auto pi = static_cast<float>(M_PI);
static constexpr auto pi_2 = static_cast<float>(M_PI_2);
static constexpr auto pi_4 = static_cast<float>(M_PI_4);

using namespace std::literals::chrono_literals;

namespace
{

iris::Vector3 walk_direction(const std::map<iris::Key, iris::KeyState> &key_map, const iris::Camera &camera)
{
    iris::Vector3 walk_direction{};

    if (key_map.at(iris::Key::W) == iris::KeyState::DOWN)
    {
        walk_direction += camera.direction();
    }

    if (key_map.at(iris::Key::S) == iris::KeyState::DOWN)
    {
        walk_direction -= camera.direction();
    }

    if (key_map.at(iris::Key::A) == iris::KeyState::DOWN)
    {
        walk_direction -= camera.right();
    }

    if (key_map.at(iris::Key::D) == iris::KeyState::DOWN)
    {
        walk_direction += camera.right();
    }

    walk_direction.y = 0.0f;
    walk_direction.normalise();
    return walk_direction;
}

}

void go(int, char **)
{
    // sanity check we're using the right version of iris for development
    static_assert(IRIS_VERSION_MAJOR == 2);
    static_assert(IRIS_VERSION_MINOR == 0);
    static_assert(IRIS_VERSION_PATCH == 0);

    std::cout << "hello trinket" << std::endl;

    std::map<iris::Key, iris::KeyState> key_map = {
        {iris::Key::W, iris::KeyState::UP},
        {iris::Key::A, iris::KeyState::UP},
        {iris::Key::S, iris::KeyState::UP},
        {iris::Key::D, iris::KeyState::UP},
    };

    // window and camera setup - we will use a 3rd person camera for this game
    auto *window = iris::Root::window_manager().create_window(800u, 800u);
    iris::Camera camera{iris::CameraType::PERSPECTIVE, window->width(), window->height(), 10000u};
    camera.set_position({0.0f, 0.0f, 800.0f});
    auto camera_azimuth = pi_2;
    auto camera_altitude = pi_4 / 2.0f;
    camera.set_pitch(-camera_altitude);

    auto &mesh_manager = iris::Root::mesh_manager();

    // basic scene setup
    iris::Scene scene{};
    auto *player = scene.create_entity(
        nullptr, mesh_manager.cube({0.2f, 0.2f, 0.2f}), iris::Transform{{}, {}, {0.5f, 1.7f, 0.5f}});
    auto *box = scene.create_entity(
        nullptr, mesh_manager.cube({}), iris::Transform{{-10.0f, 0.0f, 0.0f}, {}, {0.5f, 1.7f, 0.5f}});
    auto *enemy = scene.create_entity(
        nullptr, mesh_manager.cube({1.0f, 0.0f, 0.0f}), iris::Transform{{10.0f, 0.0f, 0.0f}, {}, {2.0f}});
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

    ps->create_rigid_body(ground->position(), ps->create_box_collision_shape({1000.0f}), iris::RigidBodyType::STATIC);
    ps->create_rigid_body(
        box->position(), ps->create_box_collision_shape({0.5f, 1.7f, 0.5f}), iris::RigidBodyType::STATIC);
    const auto *enemy_body =
        ps->create_rigid_body(enemy->position(), ps->create_box_collision_shape({2.0f}), iris::RigidBodyType::NORMAL);
    auto *character_controller = ps->create_character_controller();
    character_controller->reposition(player->position(), {});

    iris::RenderPass render_pass{&scene, &camera, nullptr, sky_box};
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
            auto running = true;

            auto event = window->pump_event();
            while (event)
            {
                if (event->is_key(iris::Key::ESCAPE) || event->is_quit())
                {
                    running = false;
                }
                else if (event->is_mouse())
                {
                    static const auto sensitivity = 0.0025f;
                    const auto mouse = event->mouse();

                    //  adjust camera azimuth and ensure we are still pointing at the player
                    camera_azimuth += mouse.delta_x * sensitivity;
                    camera.adjust_yaw(mouse.delta_x * sensitivity);

                    //  adjust camera altitude and ensure we are still pointing at the player
                    // we clamp the altitude [0, pi/2] to ensure no weirdness happens
                    camera_altitude += -mouse.delta_y * sensitivity;
                    if (camera_altitude <= 0.0f)
                    {
                        camera_altitude = 0.0f;
                    }
                    else if (camera_altitude >= pi_2)
                    {
                        camera_altitude = pi_2;
                    }
                    else
                    {
                        // only update pitch if altitude changed
                        camera.adjust_pitch(mouse.delta_y * sensitivity);
                    }
                }
                else if (event->is_key())
                {
                    const auto key = event->key();
                    key_map[key.key] = key.state;
                }

                event = window->pump_event();
            }

            character_controller->set_walk_direction(walk_direction(key_map, camera));
            player->set_position(character_controller->position());

            static constexpr auto distance = 30.0f;

            // we store the camera position as polar coordinates, which means it moves around a unit sphere centered on
            // the player convert back to cartesian coords
            const iris::Vector3 offset{
                distance * std::sin(pi_2 - camera_altitude) * std::cos(camera_azimuth),
                distance * std::cos(pi_2 - camera_altitude),
                distance * std::sin(pi_2 - camera_altitude) * std::sin(camera_azimuth)};

            // update camera position to always follow player
            const iris::Vector3 new_camera_pos = player->position() + offset;
            camera.set_position(new_camera_pos);

            // make player always face in direction of camera
            iris::Quaternion player_orientation{{0.0f, 1.0f, 0.0f}, -camera_azimuth};
            player_orientation.normalise();
            player->set_orientation(player_orientation);

            // update enemy
            enemy->set_position(enemy_body->position());
            enemy->set_orientation(enemy_body->orientation());

            window->render();

            return running;
        }};

    looper.run();
}

int main(int argc, char **argv)
{
    iris::start(argc, argv, go);
    return 0;
}
