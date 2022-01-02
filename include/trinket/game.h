////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <any>

#include "message_type.h"
#include "subscriber.h"

namespace trinket
{

class Game : public Subscriber
{
  public:
    Game();

    void run();

    void handle_message(MessageType message_type, const std::any &data) override;

  private:
    bool running_;
};

}
