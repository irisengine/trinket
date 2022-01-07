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

#include "config.h"
#include "config_option.h"

namespace trinket
{

class YamlConfig : public Config
{
  public:
    YamlConfig(const std::string &config_file);
    ~YamlConfig() override = default;

    std::string string_option(ConfigOption option) override;
    std::uint32_t uint32_option(ConfigOption option) override;
    bool bool_option(ConfigOption option) override;

  private:
    using ConfigTypes = std::variant<std::string, std::uint32_t, bool>;
    std::unordered_map<ConfigOption, ConfigTypes> options_;
};

}
