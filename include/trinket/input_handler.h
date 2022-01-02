////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "iris/graphics/window.h"

#include "game_object.h"
#include "publisher.h"

namespace trinket
{

class InputHandler : public GameObject, Publisher
{
  public:
    InputHandler(iris::Window *window);
    void update() override;

  private:
    iris::Window *window_;
};

}