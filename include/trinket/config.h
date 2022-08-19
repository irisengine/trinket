////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "config_option.h"

namespace trinket
{

/**
 * Interface for loading configuration options. It is expected that an exception will be thrown if a config option is
 * missing.
 */
class Config
{
  public:
    virtual ~Config() = default;

    /**
     * Get option as a string.
     *
     * @param option
     *   Option to get.
     *
     * @returns
     *   Option as a string.
     */
    virtual std::string string_option(ConfigOption option) = 0;

    /**
     * Get option as a uint32.
     *
     * @param option
     *   Option to get.
     *
     * @returns
     *   Option as a uint32.
     */
    virtual std::uint32_t uint32_option(ConfigOption option) = 0;

    /**
     * Get option as a bool.
     *
     * @param option
     *   Option to get.
     *
     * @returns
     *   Option as a bool.
     */
    virtual bool bool_option(ConfigOption option) = 0;

    /**
     * Get option as a collection of strings.
     *
     * @param option
     *   Option to get.
     *
     * @returns
     *   Option as a collection of strings.
     */
    virtual std::vector<std::string> string_array_option(ConfigOption option) = 0;
};

}
