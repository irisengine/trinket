////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <memory>

#include "iris/core/resource_loader.h"
#include "iris/core/start.h"
#include "iris/iris_version.h"

#include "game.h"
#include "yaml_config.h"

void go(int, char **)
{
    // sanity check we're using the right version of iris for development
    static_assert(IRIS_VERSION_MAJOR == 2);
    static_assert(IRIS_VERSION_MINOR == 1);
    static_assert(IRIS_VERSION_PATCH == 0);

    std::cout << "hello trinket" << std::endl;

    iris::ResourceLoader::instance().set_root_directory("assets");

    trinket::Game game{std::make_unique<trinket::YamlConfig>("config.yml")};
    game.run();
}

int main(int argc, char **argv)
{
    iris::start(argc, argv, go);
    return 0;
}
