////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <chrono>

namespace trinket
{

/**
 * Generic object for the game than can be updated each tick.
 */
class GameObject
{
  public:
    virtual ~GameObject() = default;

    /**
     * Update object.
     *
     * @param elapsed
     *   Time since last update.
     */
    virtual void update(std::chrono::microseconds elapsed) = 0;
};

}
