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
    KEY_PRESS,
    WEAPON_COLLISION,
};

}
