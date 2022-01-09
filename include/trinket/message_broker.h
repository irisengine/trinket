////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <any>
#include <set>
#include <unordered_map>
#include <vector>

#include "message_type.h"

namespace trinket
{

class Subscriber;

class MessageBroker
{
  public:
    static MessageBroker &instance();

    void subscribe(Subscriber *subscriber, MessageType message_type);
    void unsubscribe(Subscriber *subscriber, MessageType message_type);
    void publish(MessageType message_type, const std::any &data) const;

  private:
    MessageBroker();

    std::unordered_map<MessageType, std::vector<Subscriber *>> subscriptions_;
};

}
