////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include "publisher.h"

#include <any>

#include "message_broker.h"
#include "message_type.h"

namespace trinket
{

void Publisher::publish(MessageType message_type, const std::any &data)
{
    MessageBroker::instance().publish(message_type, data);
}

}
