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

/**
 * Singleton class that is responsible for sending messages i.e connecting publishers to subscribers.
 */
class MessageBroker
{
  public:
    /**
     * Get the single instance of this class.
     *
     * @returns
     *   Single instance.
     */
    static MessageBroker &instance();

    /**
     * Subscribe a subscriber to a message type.
     *
     * @param subscriber
     *   Object wanting to subscribe.
     *
     * @param message_type
     *   Type of message to subscribe to.
     */
    void subscribe(Subscriber *subscriber, MessageType message_type);

    /**
     * Unsubscribe a subscriber to a message type.
     *
     * @param subscriber
     *   Object wanting to unsubscribe.
     *
     * @param message_type
     *   Type of message to unsubscribe to.
     */
    void unsubscribe(Subscriber *subscriber, MessageType message_type);

    /*
     * Publish a message to all subscribers.
     *
     * @param message_type
     *   Type of message being sent (will be sent to subscribers of this type).
     *
     * @param data
     *   Any data for the message.
     */
    void publish(MessageType message_type, const std::any &data) const;

  private:
    /**
     * Private constructor to force access through singleton.
     */
    MessageBroker();

    /** Collection of subscriptions. */
    std::unordered_map<MessageType, std::vector<Subscriber *>> subscriptions_;
};

}
