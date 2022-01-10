////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <memory>
#include <vector>

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
    static_assert(IRIS_VERSION_MAJOR == 3);
    static_assert(IRIS_VERSION_MINOR == 0);
    static_assert(IRIS_VERSION_PATCH == 0);

    std::cout << "hello trinket" << std::endl;

    iris::ResourceLoader::instance().set_root_directory("assets");
    auto config = std::make_unique<trinket::YamlConfig>("config.yml");

    if (const auto &graphics_api = config->string_option(trinket::ConfigOption::GRAPHICS_API);
        graphics_api != "default")
    {
        iris::Root::set_graphics_api(graphics_api);
    }

    std::vector<std::unique_ptr<trinket::ZoneLoader>> zone_loaders{};
    for (const auto &zone_file : config->string_array_option(trinket::ConfigOption::ZONE_LOADERS))
    {
        zone_loaders.emplace_back(std::make_unique<trinket::YamlZoneLoader>(zone_file));
    }

    trinket::Game game{std::move(config), std::move(zone_loaders)};
    game.run();
}

int main(int argc, char **argv)
{
    iris::start(argc, argv, go);
    return 0;
}
