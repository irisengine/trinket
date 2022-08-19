////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <cstdint>

namespace trinket
{

class Quest
{
  public:
    virtual ~Quest() = default;

    virtual bool is_complete() const = 0;
    virtual std::uint32_t completion_xp() const = 0;
};

}
