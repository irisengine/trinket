////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <memory>
#include <string>
#include <vector>

#include "iris/core/quaternion.h"
#include "iris/core/vector3.h"
#include "iris/graphics/render_graph/render_graph.h"
#include "iris/physics/collision_shape.h"
#include "yaml-cpp/yaml.h"

#include "zone_loader.h"

namespace trinket
{

class YamlZoneLoader : public ZoneLoader
{
  public:
    YamlZoneLoader(const std::string &zone_file);
    ~YamlZoneLoader() override = default;
    std::string name() override;
    iris::Vector3 player_start_position() override;
    std::vector<StaticGeometry> static_geometry() override;
    std::vector<EnemyInfo> enemies() override;
    std::tuple<iris::Transform, std::string> portal() override;

  private:
    YAML::Node yaml_file_;
};

}
