////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <cstdint>

namespace trinket
{

/** Interface for quest. */
class Quest
{
  public:
    virtual ~Quest() = default;

    /**
     * Check if quest is completed.
     *
     * @returns
     *   True if quest is completed, otherwise false.
     */
    virtual bool is_complete() const = 0;

    /**
     * XP to award on quest completion.
     *
     * @returns
     *   Reward XP.
     */
    virtual std::uint32_t completion_xp() const = 0;
};

}
