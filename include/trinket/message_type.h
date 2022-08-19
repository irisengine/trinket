////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <cstdint>

namespace trinket
{

enum class MessageType : std::uint16_t
{
    QUIT,
    MOUSE_MOVE,
    MOUSE_BUTTON_PRESS,
    KEY_PRESS,
    SCROLL_WHEEL,
    WEAPON_COLLISION,
    ENEMY_ATTACK,
    PLAYER_HEALTH_CHANGE,
    KILLED_ENEMY,
    LEVEL_PROGRESS,
    OBJECT_COLLISION,
    QUEST_COMPLETE,
    PLAYER_DIED,
};

}
