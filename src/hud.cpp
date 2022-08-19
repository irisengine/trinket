////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include "hud.h"

#include <any>
#include <chrono>
#include <cstdint>

#include "iris/core/camera.h"
#include "iris/core/colour.h"
#include "iris/core/error_handling.h"
#include "iris/core/root.h"
#include "iris/core/transform.h"
#include "iris/graphics/mesh_manager.h"
#include "iris/graphics/render_pass.h"
#include "iris/graphics/render_target.h"
#include "iris/graphics/scene.h"
#include "iris/graphics/single_entity.h"
#include "iris/graphics/texture_manager.h"
#include "iris/graphics/window.h"
#include "iris/graphics/window_manager.h"

#include "game_object.h"
#include "message_type.h"
#include "subscriber.h"

namespace trinket
{

HUD::HUD(float starting_health, std::uint32_t width, std::uint32_t height, iris::Scene *scene)
    : health_bar_(nullptr)
    , level_progress_bar_(nullptr)
    , camera_(iris::CameraType::ORTHOGRAPHIC, width, height)
    , width_(width)
    , height_(height)
    , starting_health_(starting_health)
{
    const auto offset = 20.0f;

    health_bar_ = scene->create_entity<iris::SingleEntity>(
        nullptr,
        iris::Root::mesh_manager().sprite({1.0f, 0.0f, 0.0f}),
        iris::Transform({0.0f, height - offset, 1.0f}, {}, {width - 40.0f, 10.0f, 0.0f}));

    level_progress_bar_ = scene->create_entity<iris::SingleEntity>(
        nullptr,
        iris::Root::mesh_manager().sprite({1.0f, 1.0f, 0.0f}),
        iris::Transform({0.0f, offset - height, 1.0f}, {}, {0.0f, 10.0f, 0.0f}));

    subscribe(MessageType::PLAYER_HEALTH_CHANGE);
    subscribe(MessageType::LEVEL_PROGRESS);
}

void HUD::update(std::chrono::microseconds)
{
}

void HUD::handle_message(MessageType message_type, const std::any &data)
{
    switch (message_type)
    {
        case MessageType::PLAYER_HEALTH_CHANGE:
        {
            const auto remaining_health = std::any_cast<float>(data);
            const auto bar_width = (width_ - 4.0f) * (remaining_health / starting_health_);

            health_bar_->set_scale({bar_width, 10.0f, 1.0f});
            break;
        }
        case MessageType::LEVEL_PROGRESS:
        {
            const auto progress = std::any_cast<float>(data);
            const auto bar_width = (width_ - 40.0f) * progress;

            level_progress_bar_->set_scale({bar_width, 10.0f, 1.0f});
            break;
        }
        default: break;
    }
}

}
