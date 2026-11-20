cmake_minimum_required(VERSION 3.16)
project(ThreeDSpiralGalaxy LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(SDL2 REQUIRED)

add_executable(galaxy src/galaxy.cpp)
target_link_libraries(galaxy PRIVATE SDL2::SDL2)
