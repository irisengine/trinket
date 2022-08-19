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

/**
 * Implementation of ZoneLoader for YAM<L files.
 */
class YamlZoneLoader : public ZoneLoader
{
  public:
    /**
     * Create a new YamlZoneLoader.
     *
     * @param zone_file
     *   YAML file to parse.
     */
    YamlZoneLoader(const std::string &zone_file);

    /**
     * Get the name of the zone.
     *
     * @returns
     *   Zone name.
     */
    std::string name() override;

    /**
     * Get player start position.
     *
     * @returns
     *   Player start position.
     */
    iris::Vector3 player_start_position() override;

    /**
     * Load static geometry.
     *
     * @param ps
     *   Physics system.
     *
     * @param scene
     *   Scene to load into.
     *
     * @param render_pipeline
     *   Render pipeline to use.
     */
    void load_static_geometry(iris::PhysicsSystem *ps, iris::Scene *scene, iris::RenderPipeline &render_pipeline)
        override;

    /**
     * Load enemies.
     *
     * @param ps
     *   Physics system.
     *
     * @param scene
     *   Scene to load into.
     *
     * @param render_pipeline
     *   Render pipeline to use.
     *
     * @param game_objects
     *   Collection of game objects to add enemies to.
     *
     * @param player
     *   Pointer to player object.
     *
     * @param camera
     *   Pointer to camera object,
     */
    void load_enemies(
        iris::PhysicsSystem *ps,
        iris::Scene *scene,
        iris::RenderPipeline &render_pipeline,
        std::vector<std::unique_ptr<GameObject>> &game_objects,
        Player *player,
        ThirdPersonCamera *camera) override;

    /**
     * Get portal data.
     *
     * @returns
     *   Tuple of portal transform and name of next zone.
     */
    std::tuple<iris::Transform, std::string> portal() override;

  private:
    /** YAML node. */
    YAML::Node yaml_file_;
};

}
