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
#include "iris/graphics/scene.h"
#include "iris/physics/physics_system.h"

#include "game_object.h"
#include "player.h"
#include "third_person_camera.h"

namespace trinket
{

class ZoneLoader
{
  public:
    virtual ~ZoneLoader() = default;
    virtual std::string name() = 0;
    virtual iris::Vector3 player_start_position() = 0;
    virtual void load_static_geometry(iris::PhysicsSystem *ps, iris::Scene &scene) = 0;
    virtual void load_enemies(
        iris::PhysicsSystem *ps,
        iris::Scene &scene,
        std::vector<std::unique_ptr<GameObject>> &game_objects,
        Player *player,
        ThirdPersonCamera *camera) = 0;
    virtual std::tuple<iris::Transform, std::string> portal() = 0;
};

}
