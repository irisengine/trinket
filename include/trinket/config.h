////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <cstdint>
#include <string>

#include "config_option.h"

namespace trinket
{

class Config
{
  public:
    virtual ~Config() = default;
    virtual std::string string_option(ConfigOption option) = 0;
    virtual std::uint32_t uint32_option(ConfigOption option) = 0;
    virtual bool bool_option(ConfigOption option) = 0;
};

}
