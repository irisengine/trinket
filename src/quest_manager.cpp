////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "quest_manager.h"

#include <chrono>

#include "message_type.h"

namespace trinket
{

void QuestManager::update(std::chrono::microseconds)
{
    auto iter =
        std::remove_if(std::begin(quests_), std::end(quests_), [](const auto &quest) { return quest->is_complete(); });

    std::for_each(iter, std::end(quests_), [&](const auto &quest) {
        publish(MessageType::QUEST_COMPLETE, {quest->completion_xp()});
    });

    quests_.erase(iter, std::end(quests_));
}

}
