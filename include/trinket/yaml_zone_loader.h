////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <memory>
#include <string>
#include <vector>

#include "iris/core/vector3.h"
#include "iris/graphics/render_pipeline.h"
#include "iris/graphics/scene.h"
#include "iris/physics/physics_system.h"

#include "yaml-cpp/yaml.h"

#include "game_object.h"
#include "player.h"
#include "third_person_camera.h"
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
    void load_static_geometry(iris::PhysicsSystem *ps, iris::Scene *scene, iris::RenderPipeline &render_pipeline)
        override;
    void load_enemies(
        iris::PhysicsSystem *ps,
        iris::Scene *scene,
        iris::RenderPipeline &render_pipeline,
        std::vector<std::unique_ptr<GameObject>> &game_objects,
        Player *player,
        ThirdPersonCamera *camera) override;
    std::tuple<iris::Transform, std::string> portal() override;

  private:
    YAML::Node yaml_file_;
};

}
