////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include "kill_enemy_quest.h"

#include <any>
#include <cstdint>

#include "message_type.h"
#include "quest.h"
#include "subscriber.h"

namespace trinket
{

KillEnemyQuest::KillEnemyQuest(std::uint32_t enemy_count)
    : enemy_count_(enemy_count)
    , complete_(false)
{
    subscribe(MessageType::KILLED_ENEMY);
}

bool KillEnemyQuest::is_complete() const
{
    return complete_;
}

std::uint32_t KillEnemyQuest::completion_xp() const
{
    return 50u;
}

void KillEnemyQuest::handle_message(MessageType message_type, const std::any &data)
{
    switch (message_type)
    {
        case MessageType::KILLED_ENEMY:
        {
            if (!complete_)
            {
                --enemy_count_;
                if (enemy_count_ == 0u)
                {
                    complete_ = true;
                }
            }
            break;
        }
        default: break;
    }
}

}
