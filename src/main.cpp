////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include <iostream>

#include "iris/core/start.h"

void go(int, char **)
{
    std::cout << "hello trinket" << std::endl;
}

int main(int argc, char **argv)
{
    iris::start(argc, argv, go);
    return 0;
}
