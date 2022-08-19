////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <any>
#include <cstdint>

#include "message_type.h"
#include "quest.h"
#include "subscriber.h"

namespace trinket
{

/**
 * Implementation of quest for killing a specific number of enemies.
 */
class KillEnemyQuest : public Quest, Subscriber
{
  public:
    /**
     * Create a new KillEnemyQuest object.
     *
     * @param enemy_count
     *   Number of enemies to kill to complete quest.
     */
    KillEnemyQuest(std::uint32_t enemy_count);

    /**
     * Check if quest is completed.
     *
     * @returns
     *   True if quest is completed, otherwise false.
     */
    bool is_complete() const override;

    /**
     * XP to award on quest completion.
     *
     * @returns
     *   Reward XP.
     */
    std::uint32_t completion_xp() const override;

    /**
     * Message handler.
     *
     * @param message_type
     *   Type of message being sent.
     *
     * @param data
     *   Any data for the message.
     */
    void handle_message(MessageType message_type, const std::any &data) override;

  private:
    /** Number of enemies to kill. */
    std::uint32_t enemy_count_;

    /** Flag indicating if quest is complete. */
    bool complete_;
};

}
