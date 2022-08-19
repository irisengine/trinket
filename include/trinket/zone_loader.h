////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <memory>
#include <string>
#include <tuple>
#include <vector>

#include "iris/core/vector3.h"
#include "iris/graphics/render_pipeline.h"
#include "iris/graphics/scene.h"
#include "iris/physics/physics_system.h"

#include "game_object.h"
#include "player.h"
#include "third_person_camera.h"

namespace trinket
{

/**
 * Interface to load zone data.
 */
class ZoneLoader
{
  public:
    virtual ~ZoneLoader() = default;

    /**
     * Get the name of the zone.
     *
     * @returns
     *   Zone name.
     */
    virtual std::string name() = 0;

    /**
     * Get player start position.
     *
     * @returns
     *   Player start position.
     */
    virtual iris::Vector3 player_start_position() = 0;

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
    virtual void load_static_geometry(
        iris::PhysicsSystem *ps,
        iris::Scene *scene,
        iris::RenderPipeline &render_pipeline) = 0;

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
    virtual void load_enemies(
        iris::PhysicsSystem *ps,
        iris::Scene *scene,
        iris::RenderPipeline &render_pipeline,
        std::vector<std::unique_ptr<GameObject>> &game_objects,
        Player *player,
        ThirdPersonCamera *camera) = 0;

    /**
     * Get portal data.
     *
     * @returns
     *   Tuple of portal transform and name of next zone.
     */
    virtual std::tuple<iris::Transform, std::string> portal() = 0;
};

}
