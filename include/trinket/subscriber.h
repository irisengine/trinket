////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <any>
#include <set>

#include "message_type.h"

namespace trinket
{

class Subscriber
{
  public:
    virtual ~Subscriber();
    virtual void handle_message(MessageType message_type, const std::any &data) = 0;

  protected:
    void subscribe(MessageType message_type);

    std::set<MessageType> subscribed_;
};

}
