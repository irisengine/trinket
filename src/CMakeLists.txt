set(INCLUDE_ROOT "${PROJECT_SOURCE_DIR}/include/trinket")

add_executable(trinket
  ${INCLUDE_ROOT}/character_controller.h
  ${INCLUDE_ROOT}/config.h
  ${INCLUDE_ROOT}/config_option.h
  ${INCLUDE_ROOT}/enemy.h
  ${INCLUDE_ROOT}/game.h
  ${INCLUDE_ROOT}/game_object.h
  ${INCLUDE_ROOT}/hud.h
  ${INCLUDE_ROOT}/input_handler.h
  ${INCLUDE_ROOT}/kill_enemy_quest.h
  ${INCLUDE_ROOT}/maths.h
  ${INCLUDE_ROOT}/message_broker.h
  ${INCLUDE_ROOT}/player.h
  ${INCLUDE_ROOT}/publisher.h
  ${INCLUDE_ROOT}/quest.h
  ${INCLUDE_ROOT}/quest_manager.h
  ${INCLUDE_ROOT}/subscriber.h
  ${INCLUDE_ROOT}/third_person_camera.h
  ${INCLUDE_ROOT}/yaml_config.h
  ${INCLUDE_ROOT}/yaml_zone_loader.h
  ${INCLUDE_ROOT}/zone_loader.h
  character_controller.cpp
  enemy.cpp
  game.cpp
  hud.cpp
  input_handler.cpp
  kill_enemy_quest.cpp
  main.cpp
  message_broker.cpp
  player.cpp
  publisher.cpp
  quest_manager.cpp
  subscriber.cpp
  third_person_camera.cpp
  yaml_config.cpp
  yaml_zone_loader.cpp)

target_include_directories(trinket PRIVATE ${INCLUDE_ROOT})

find_package(iris REQUIRED PATHS ${PROJECT_SOURCE_DIR}/third_party/iris/lib/cmake/iris NO_DEFAULT_PATH)

target_link_libraries(trinket iris::iris yaml-cpp)

if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
  set_target_properties(trinket PROPERTIES MSVC_RUNTIME_LIBRARY "MultiThreadedDebug")
  set_target_properties(yaml-cpp PROPERTIES MSVC_RUNTIME_LIBRARY "MultiThreadedDebug")
endif()
