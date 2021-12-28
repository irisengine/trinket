add_executable(trinket main.cpp)

find_package(iris REQUIRED PATHS ${PROJECT_SOURCE_DIR}/third_party/iris/lib/cmake/iris NO_DEFAULT_PATH)

target_link_libraries(trinket iris::iris)