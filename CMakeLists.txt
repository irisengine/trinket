cmake_minimum_required(VERSION 3.18)

project(
  trinket
  VERSION "0.1.0"
  DESCRIPTION "Game built with iris"
  LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)

include(FetchContent)

FetchContent_Declare(
  yaml-cpp
  GIT_REPOSITORY https://github.com/jbeder/yaml-cpp.git
  GIT_TAG yaml-cpp-0.7.0)
FetchContent_GetProperties(yaml-cpp)
if(NOT yaml-cpp_POPULATED)
  FetchContent_Populate(yaml-cpp)
  add_subdirectory(${yaml-cpp_SOURCE_DIR} ${yaml-cpp_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()

add_subdirectory("src")

file(COPY assets DESTINATION ${PROJECT_BINARY_DIR}/src)