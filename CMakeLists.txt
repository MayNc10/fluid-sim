cmake_minimum_required(VERSION 3.7)

list(APPEND CMAKE_PREFIX_PATH "C:\\Users\\naten\\Documents\\SDL2-devel-2.28.5-VC\\SDL2-2.28.5\\cmake")
project(fluid_sim)

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

add_executable(fluid_sim main.cpp)
target_link_libraries(fluid_sim ${SDL2_LIBRARIES})