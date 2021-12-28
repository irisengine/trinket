////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include <iostream>

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

void go(int, char **)
{
    static_assert(IRIS_VERSION_MAJOR == 2);
    static_assert(IRIS_VERSION_MINOR == 0);
    static_assert(IRIS_VERSION_PATCH == 0);

    std::cout << "hello trinket" << std::endl;

    auto *window = iris::Root::window_manager().create_window(800u, 800u);
    iris::Camera camera{iris::CameraType::PERSPECTIVE, window->width(), window->height()};
    camera.set_position({0.0f, 5.0f, 800.0f});

    auto &mesh_manager = iris::Root::mesh_manager();

    iris::Scene scene{};
    scene.create_entity(nullptr, mesh_manager.cube({0.2f, 0.2f, 0.2f}), iris::Transform{{}, {}, {10.0f, 40.0f, 10.0f}});
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
            event = window->pump_event();
        }

        window->render();
    } while (running);
}

int main(int argc, char **argv)
{
    iris::start(argc, argv, go);
    return 0;
}
