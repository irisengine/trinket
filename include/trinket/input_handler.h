////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <chrono>

#include "iris/graphics/window.h"

#include "game_object.h"
#include "publisher.h"

namespace trinket
{

/**
 * Implementation of GameObject for handling user input.
 */
class InputHandler : public GameObject, Publisher
{
  public:
    InputHandler(iris::Window *window);

    /**
     * Update object.
     *
     * @param elapsed
     *   Time since last update.
     */
    void update(std::chrono::microseconds) override;

  private:
    /** Window for game. */
    iris::Window *window_;
};

}