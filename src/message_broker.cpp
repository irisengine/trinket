////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include "message_broker.h"

#include <any>
#include <set>
#include <unordered_map>
#include <vector>

#include "iris/core/error_handling.h"

#include "message_type.h"
#include "subscriber.h"

namespace trinket
{

MessageBroker::MessageBroker()
    : subscriptions_()
{
}

MessageBroker &MessageBroker::instance()
{
    static MessageBroker instance{};
    return instance;
}

void MessageBroker::subscribe(Subscriber *subscriber, MessageType message_type)
{
    subscriptions_[message_type].emplace_back(subscriber);
}

void MessageBroker::unsubscribe(Subscriber *subscriber, MessageType message_type)
{
    auto &subs = subscriptions_[message_type];
    subs.erase(std::remove(std::begin(subs), std::end(subs), subscriber), std::end(subs));
}

void MessageBroker::publish(MessageType message_type, const std::any &data) const
{
    if (const auto subs = subscriptions_.find(message_type); subs != std::cend(subscriptions_))
    {
        for (auto *subscriber : subs->second)
        {
            subscriber->handle_message(message_type, data);
        }
    }
}

}
