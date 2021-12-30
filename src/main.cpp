////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include <algorithm>
#include <cmath>
#include <iostream>
#include <map>

#include "iris/core/camera.h"
#include "iris/core/colour.h"
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

static constexpr auto pi = static_cast<float>(M_PI);
static constexpr auto pi_2 = static_cast<float>(M_PI_2);
static constexpr auto pi_4 = static_cast<float>(M_PI_4);

void go(int, char **)
{
    // sanity check we're using the right version of iris for development
    static_assert(IRIS_VERSION_MAJOR == 2);
    static_assert(IRIS_VERSION_MINOR == 0);
    static_assert(IRIS_VERSION_PATCH == 0);

    std::cout << "hello trinket" << std::endl;

    std::map<iris::Key, iris::KeyState> key_map = {
        {iris::Key::W, iris::KeyState::UP},
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
        nullptr, mesh_manager.cube({0.2f, 0.2f, 0.2f}), iris::Transform{{}, {}, {10.0f, 40.0f, 10.0f}});
    scene.create_entity(
        nullptr, mesh_manager.cube({}), iris::Transform{{-100.0f, 0.0f, 0.0f}, {}, {10.0f, 40.0f, 10.0f}});
    scene.create_entity(
        nullptr, mesh_manager.cube({0.0f, 1.0f, 0.0f}), iris::Transform{{0.0f, -1040.0f, 0.0f}, {}, {1000.0f}});
    scene.set_ambient_light({0.2f, 0.2f, 0.2f, 1.0f});

    const auto sky_box = iris::Root::texture_manager().create(
        iris::Colour{0.275f, 0.51f, 0.796f}, iris::Colour{0.5f, 0.5f, 0.5f}, 2048u, 2048u);

    iris::RenderPass render_pass{&scene, &camera, nullptr, sky_box};
    window->set_render_passes({render_pass});

    auto running = true;

    do
    {
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

        if (key_map[iris::Key::W] == iris::KeyState::DOWN)
        {
            // calculate velocity in direction camera is looking (ignoring the y component as the player shouldn't be
            // moving up and down)
            const auto speed = 2.0f;
            auto velocity = camera.direction();
            velocity.y = 0.0f;
            velocity.normalise();
            velocity *= speed;

            player->set_position(player->position() + velocity);
        }

        static constexpr auto distance = 400.0f;

        // we store the camera position as polar coordinates, which means it moves around a unit sphere centered on the
        // player convert back to cartesian coords
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

        window->render();
    } while (running);
}

int main(int argc, char **argv)
{
    iris::start(argc, argv, go);
    return 0;
}
