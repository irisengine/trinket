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

/**
 * Abstract class for receiving messages.
 */
class Subscriber
{
  public:
    virtual ~Subscriber();

    /**
     * Message handler.
     *
     * @param message_type
     *   Type of message being sent.
     *
     * @param data
     *   Any data for the message.
     */
    virtual void handle_message(MessageType message_type, const std::any &data) = 0;

  protected:
    /**
     * Subscribe to a message type.
     *
     * @param message_type
     *   Message type to subscribe to,
     */
    void subscribe(MessageType message_type);

    /** Collection og subscribed message types. */
    std::set<MessageType> subscribed_;
};

}
