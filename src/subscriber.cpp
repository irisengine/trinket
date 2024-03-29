////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include "subscriber.h"

#include <set>

#include "message_broker.h"
#include "message_type.h"

namespace trinket
{

Subscriber::~Subscriber()
{
    for (const auto message_type : subscribed_)
    {
        MessageBroker::instance().unsubscribe(this, message_type);
    }
}

void Subscriber::subscribe(MessageType message_type)
{
    MessageBroker::instance().subscribe(this, message_type);
    subscribed_.emplace(message_type);
}

}
