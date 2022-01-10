////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include "input_handler.h"

#include "iris/events/event.h"
#include "iris/log/log.h"

#include "message_type.h"
#include "publisher.h"

namespace trinket
{

InputHandler::InputHandler(iris::Window *window)
    : window_(window)
{
}

void InputHandler::update()
{
    auto event = window_->pump_event();
    while (event)
    {
        if (event->is_key(iris::Key::ESCAPE) || event->is_quit())
        {
            publish(MessageType::QUIT, {});
        }
        else if (event->is_mouse())
        {
            publish(MessageType::MOUSE_MOVE, {event->mouse()});
        }
        else if (event->is_key())
        {
            publish(MessageType::KEY_PRESS, {event->key()});
        }
        else if (event->is_mouse_button())
        {
            publish(MessageType::MOUSE_BUTTON_PRESS, {event->mouse_button()});
        }
        else if (event->is_scroll_wheel())
        {
            publish(MessageType::SCROLL_WHEEL, {event->scroll_wheel()});
        }

        event = window_->pump_event();
    }
}

}
