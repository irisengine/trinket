////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <any>
#include <chrono>
#include <cstdint>

#include "iris/core/camera.h"
#include "iris/graphics/cube_map.h"
#include "iris/graphics/render_pass.h"
#include "iris/graphics/render_target.h"
#include "iris/graphics/scene.h"
#include "iris/graphics/single_entity.h"

#include "game_object.h"
#include "message_type.h"
#include "subscriber.h"

namespace trinket
{

class HUD : public GameObject, Subscriber
{
  public:
    HUD(float starting_health, std::uint32_t width, std::uint32_t height, iris::Scene *scene);
    ~HUD() override = default;

    void update(std::chrono::microseconds) override;

    void handle_message(MessageType message_type, const std::any &data) override;

    iris::RenderPass render_pass(iris::RenderTarget *target);

  private:
    iris::SingleEntity *health_bar_;
    iris::SingleEntity *level_progress_bar_;
    iris::Scene *scene_;
    iris::Camera camera_;
    float width_;
    float height_;
    float starting_health_;
};

}
