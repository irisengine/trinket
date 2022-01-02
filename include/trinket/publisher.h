////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <any>

#include "message_type.h"

namespace trinket
{

class Publisher
{

  public:
    virtual ~Publisher() = default;

  protected:
    void publish(MessageType message_type, const std::any &data);
};

}
