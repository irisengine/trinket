////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <chrono>
#include <memory>
#include <type_traits>
#include <vector>

#include "game_object.h"
#include "publisher.h"
#include "quest.h"

namespace trinket
{

template <class T>
concept IsQuest = std::is_base_of_v<Quest, T>;

/**
 * GameObject to manage all plater quests.
 */
class QuestManager : public GameObject, Publisher
{
  public:
    /**
     * Create a new quest.
     *
     * @param args
     *   Args for quest object (perfectly forwarded).
     */
    template <IsQuest T, class... Args>
    void create(Args &&...args)
    {
        quests_.push_back(std::make_unique<T>(std::forward<Args>(args)...));
    }

    /**
     * Update object.
     *
     * @param elapsed
     *   Time since last update.
     */
    void update(std::chrono::microseconds elapsed) override;

  private:
    /** Collection of created quests. */
    std::vector<std::unique_ptr<Quest>> quests_;
};

}
