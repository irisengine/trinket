////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <memory>

#include "iris/core/resource_loader.h"
#include "iris/core/root.h"
#include "iris/core/start.h"
#include "iris/iris_version.h"
#include "iris/physics/physics_manager.h"

#include "game.h"
#include "yaml_config.h"
#include "yaml_zone_loader.h"

void go(int, char **)
{
    // sanity check we're using the right version of iris for development
    static_assert(IRIS_VERSION_MAJOR == 2);
    static_assert(IRIS_VERSION_MINOR == 1);
    static_assert(IRIS_VERSION_PATCH == 0);

    std::cout << "hello trinket" << std::endl;

    iris::ResourceLoader::instance().set_root_directory("assets");
    iris::Root::physics_manager().create_physics_system();
    auto config = std::make_unique<trinket::YamlConfig>("config.yml");

    if (const auto &graphics_api = config->string_option(trinket::ConfigOption::GRAPHICS_API);
        graphics_api != "default")
    {
        iris::Root::set_graphics_api(graphics_api);
    }

    trinket::Game game{std::move(config), std::make_unique<trinket::YamlZoneLoader>("town_zone.yml")};
    game.run();
}

int main(int argc, char **argv)
{
    iris::start(argc, argv, go);
    return 0;
}
