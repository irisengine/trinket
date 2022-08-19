////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <cstdint>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

#include "config.h"
#include "config_option.h"

namespace trinket
{

/**
 * Implementation of Config for YAML files.
 */
class YamlConfig : public Config
{
  public:
    /**
     * Construct a new YamlConfig.
     *
     * @param config_file.
     *   Path to YAML file.
     */
    YamlConfig(const std::string &config_file);

    /**
     * Get option as a string.
     *
     * @param option
     *   Option to get.
     *
     * @returns
     *   Option as a string.
     */
    std::string string_option(ConfigOption option) override;

    /**
     * Get option as a uint32.
     *
     * @param option
     *   Option to get.
     *
     * @returns
     *   Option as a uint32.
     */
    std::uint32_t uint32_option(ConfigOption option) override;

    /**
     * Get option as a bool.
     *
     * @param option
     *   Option to get.
     *
     * @returns
     *   Option as a bool.
     */
    bool bool_option(ConfigOption option) override;

    /**
     * Get option as a collection of strings.
     *
     * @param option
     *   Option to get.
     *
     * @returns
     *   Option as a collection of strings.
     */
    std::vector<std::string> string_array_option(ConfigOption option) override;

  private:
    using ConfigTypes = std::variant<std::string, std::uint32_t, bool, std::vector<std::string>>;

    /** Loaded config types. */
    std::unordered_map<ConfigOption, ConfigTypes> options_;
};

}
