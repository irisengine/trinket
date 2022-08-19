////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <any>

#include "message_type.h"

namespace trinket
{

/**
 * Abstract class for sending messages.
 */
class Publisher
{

  public:
    virtual ~Publisher() = default;

  protected:
    /*
     * Publish a message to all subscribers.
     *
     * @param message_type
     *   Type of message being sent (will be sent to subscribers of this type).
     *
     * @param data
     *   Any data for the message.
     */
    void publish(MessageType message_type, const std::any &data);
};

}
