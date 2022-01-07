////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <any>
#include <memory>

#include "config.h"
#include "message_type.h"
#include "subscriber.h"
#include "zone_loader.h"

namespace trinket
{

class Game : public Subscriber
{
  public:
    Game(std::unique_ptr<Config> config, std::unique_ptr<ZoneLoader> zone_loader);

    void run();

    void handle_message(MessageType message_type, const std::any &data) override;

  private:
    bool running_;
    std::unique_ptr<Config> config_;
    std::unique_ptr<ZoneLoader> zone_loader_;
};

}
