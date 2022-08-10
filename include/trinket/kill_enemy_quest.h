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

class KillEnemyQuest : public Quest, Subscriber
{
  public:
    KillEnemyQuest(std::uint32_t enemy_count);
    ~KillEnemyQuest() override = default;

    bool is_complete() const override;

    std::uint32_t completion_xp() const override;

    void handle_message(MessageType message_type, const std::any &data) override;

  private:
    std::uint32_t enemy_count_;
    bool complete_;
};

}
