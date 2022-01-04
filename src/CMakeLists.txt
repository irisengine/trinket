set(INCLUDE_ROOT "${PROJECT_SOURCE_DIR}/include/trinket")

add_executable(trinket
    ${INCLUDE_ROOT}/enemy.h
    ${INCLUDE_ROOT}/game.h
    ${INCLUDE_ROOT}/game_object.h
    ${INCLUDE_ROOT}/input_handler.h
    ${INCLUDE_ROOT}/maths.h
    ${INCLUDE_ROOT}/message_broker.h
    ${INCLUDE_ROOT}/player.h
    ${INCLUDE_ROOT}/publisher.h
    ${INCLUDE_ROOT}/subscriber.h
    ${INCLUDE_ROOT}/third_person_camera.h
    enemy.cpp
    game.cpp
    input_handler.cpp
    main.cpp
    message_broker.cpp
    player.cpp
    publisher.cpp
    subscriber.cpp
    third_person_camera.cpp)

target_include_directories(trinket PRIVATE ${INCLUDE_ROOT})

find_package(iris REQUIRED PATHS ${PROJECT_SOURCE_DIR}/third_party/iris/lib/cmake/iris NO_DEFAULT_PATH)

target_link_libraries(trinket iris::iris)
