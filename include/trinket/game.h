////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <any>
#include <memory>
#include <string>
#include <vector>

#include "iris/graphics/window.h"
#include "iris/physics/rigid_body.h"

#include "config.h"
#include "message_type.h"
#include "subscriber.h"
#include "zone_loader.h"

namespace trinket
{

class Game : public Subscriber
{
  public:
    Game(std::unique_ptr<Config> config, std::vector<std::unique_ptr<ZoneLoader>> &&zone_loaders);

    void run();

    void handle_message(MessageType message_type, const std::any &data) override;

  private:
    void run_zone();

    bool running_;
    std::unique_ptr<Config> config_;
    std::vector<std::unique_ptr<ZoneLoader>> zone_loaders_;
    ZoneLoader *current_zone_;
    ZoneLoader *next_zone_;
    iris::Window *window_;
    iris::RigidBody *portal_;
    std::string portal_destination_;
};

}
