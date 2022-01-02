////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <any>
#include <cstdint>
#include <map>

#include "iris/core/camera.h"
#include "iris/events/event.h"

#include "game_object.h"
#include "message_type.h"
#include "player.h"
#include "subscriber.h"

namespace trinket
{

class ThirdPersonCamera : public GameObject, Subscriber
{
  public:
    ThirdPersonCamera(Player *player, std::uint32_t width, std::uint32_t height);
    ~ThirdPersonCamera() override = default;
    void update() override;
    iris::Camera *camera();

  protected:
    void handle_message(MessageType message_type, const std::any &data) override;

  private:
    Player *player_;
    iris::Camera camera_;
    std::map<iris::Key, iris::KeyState> key_map_;
    float azimuth_;
    float altitude_;
};

}
