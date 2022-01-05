////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include "yaml_config.h"

#include <cstdint>
#include <string>
#include <variant>

#include "iris/core/error_handling.h"
#include "iris/core/exception.h"
#include "iris/core/resource_loader.h"
#include "yaml-cpp/yaml.h"

#include "config_option.h"

namespace
{

template <class T, class S>
T get_config_option(std::unordered_map<trinket::ConfigOption, S> &options, trinket::ConfigOption option)
{
    iris::expect(options.contains(option), "missing config option");
    iris::expect(std::holds_alternative<T>(options[option]), "incorrect config option type");

    return std::get<T>(options[option]);
}

}

namespace trinket
{

YamlConfig::YamlConfig(const std::string &config_file)
    : options_()
{
    const auto config_file_data = iris::ResourceLoader::instance().load(config_file);
    std::string config_file_str(reinterpret_cast<const char *>(config_file_data.data()), config_file_data.size());

    const auto yaml_config = ::YAML::Load(config_file_str);

    options_[ConfigOption::SCREEN_WIDTH] = yaml_config["screen_width"].as<std::uint32_t>();
    options_[ConfigOption::SCREEN_HEIGHT] = yaml_config["screen_height"].as<std::uint32_t>();
    options_[ConfigOption::GRAPHICS_API] = yaml_config["graphics_api"].as<std::string>();
}

std::string YamlConfig::string_option(ConfigOption option)
{
    return get_config_option<std::string>(options_, option);
}

std::uint32_t YamlConfig::uint32_option(ConfigOption option)
{
    return get_config_option<std::uint32_t>(options_, option);
}

}
